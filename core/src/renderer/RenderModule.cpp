// ReSharper disable CppVariableCanBeMadeConstexpr
// ReSharper disable CppMemberFunctionMayBeConst
#define GLM_ENABLE_EXPERIMENTAL
#include "RenderModule.hpp"

#include "assets/AssetModule.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "shaders/Shader.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/ShaderUtils.hpp"

#include "window/WindowModule.hpp"


namespace siren::core
{
bool RenderModule::Init()
{
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_cameraBuffer = CreateOwn<Buffer>(nullptr, sizeof(CameraUBO), BufferUsage::Dynamic);
    m_lightBuffer  = CreateOwn<Buffer>(nullptr, sizeof(LightUBO), BufferUsage::Dynamic);

    // load shaders
    {
        m_shaderLibrary.Import("ass://shaders/pbr.sshg", "PBR");
        m_shaderLibrary.Import("ass://shaders/grid.sshg", "Grid");
        m_shaderLibrary.Import("ass://shaders/skyLight.sshg", "SkyBox");
    }

    // pbr pipeline
    {
        GraphicsPipeline::Properties props;
        props.layout.SetLayout(
            {
                VertexAttribute::Position,
                VertexAttribute::Normal,
                VertexAttribute::Tangent,
                VertexAttribute::Bitangent,
                VertexAttribute::Texture
            }
        );
        props.topology        = PrimitiveTopology::Triangles;
        props.alphaMode       = AlphaMode::Opaque;
        props.depthFunction   = DepthFunction::Less;
        props.backFaceCulling = true;
        props.depthTest       = true;
        props.depthWrite      = true;
        props.shader          = m_shaderLibrary.Get("PBR");
        m_pipelines.pbr       = CreateRef<GraphicsPipeline>(props, "PBR Pipeline");
    }

    // skybox pipeline
    {
        GraphicsPipeline::Properties props;
        props.layout.SetLayout({ VertexAttribute::Position });
        props.topology        = PrimitiveTopology::Triangles;
        props.alphaMode       = AlphaMode::Opaque;
        props.depthFunction   = DepthFunction::LessEqual;
        props.backFaceCulling = false;
        props.depthTest       = false;
        props.depthWrite      = false;
        props.shader          = m_shaderLibrary.Get("SkyBox");
        m_pipelines.skybox    = CreateRef<GraphicsPipeline>(props, "SkyBox Pipeline");
    }

    m_unitCube = primitive::Generate(CubeParams{ }, m_pipelines.skybox->GetLayout());

    return true;
}

void RenderModule::Shutdown()
{
    // nothing for now
}

void RenderModule::BeginFrame(const RenderInfo& renderInfo)
{
    m_stats.Reset();
    m_renderInfo = renderInfo;

    CameraUBO cameraUbo;
    cameraUbo.projectionView = renderInfo.cameraInfo.projectionMatrix * renderInfo.cameraInfo.viewMatrix;
    cameraUbo.cameraPosition = renderInfo.cameraInfo.position;
    m_cameraBuffer->Update(&cameraUbo, sizeof(CameraUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer->GetID());

    LightUBO lightUbo;
    lightUbo.pointLights           = renderInfo.lightInfo.pointLights;
    lightUbo.directionalLights     = renderInfo.lightInfo.directionalLights;
    lightUbo.spotLights            = renderInfo.lightInfo.spotLights;
    lightUbo.pointLightCount       = renderInfo.lightInfo.pointLightCount;
    lightUbo.directionalLightCount = renderInfo.lightInfo.directionalLightCount;
    lightUbo.spotLightCount        = renderInfo.lightInfo.spotLightCount;

    m_lightBuffer->Update(&lightUbo, sizeof(LightUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer->GetID());
}

void RenderModule::EndFrame()
{
    // todo: we should really add a SubmitSkybox fn, but that requires a more complex BindMaterial()
}

void RenderModule::BeginPass(const Ref<FrameBuffer>& frameBuffer, const glm::vec4& clearColor)
{
    m_currentFramebuffer = frameBuffer.get();

    if (m_currentFramebuffer) {
        frameBuffer->Bind();
        frameBuffer->SetViewport();
    } else {
        const auto size = window().GetSize();
        glViewport(0, 0, size.x, size.y);
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderModule::EndPass()
{
    // todo: sort based on depth here too!
    std::sort(
        m_drawQueue.begin(),
        m_drawQueue.end(),
        [] (const DrawCommand& left, const DrawCommand& right) {
            if (left.pipeline != right.pipeline) { // we sort via ptr comparison
                return left.pipeline < right.pipeline;
            }
            return left.material < right.material;
        }
    );

    // todo:
    //      - SubmitSkylight() or something
    //      - Putting this at the end of the pass causes weird depth issues so idk why that happens
    DrawSkyLight();

    const Buffer* lastVertices           = nullptr;
    const Buffer* lastIndices            = nullptr;
    const GraphicsPipeline* lastPipeline = nullptr;
    const Material* lastMaterial         = nullptr;

    for (const auto& cmd : m_drawQueue) {
        if (!cmd) { continue; }

        if (cmd.pipeline != lastPipeline) {
            cmd.pipeline->Bind();
            lastPipeline = cmd.pipeline;
            m_stats.pipelineBinds++;
        }

        if (cmd.material != lastMaterial) {
            BindMaterial(cmd.material, cmd.pipeline->GetShader().get());
            lastMaterial = cmd.material;
        }

        cmd.pipeline->GetShader()->SetUniform("u_model", m_transforms[cmd.transformIndex]);

        const GLenum top = topologyToGlEnum(cmd.pipeline->GetTopology());

        if (cmd.vertices != lastVertices) {
            glVertexArrayVertexBuffer(
                cmd.pipeline->GetVertexArrayID(),
                0,
                cmd.vertices->GetID(),
                0,
                cmd.pipeline->GetStride()
            );
            lastVertices = cmd.vertices;
        }

        if (cmd.indices != lastIndices) {
            glVertexArrayElementBuffer(cmd.pipeline->GetVertexArrayID(), cmd.indices->GetID());
            lastIndices = cmd.indices;
        }

        glDrawElements(top, cmd.indexCount, GL_UNSIGNED_INT, nullptr);
        m_stats.drawCalls++;
        m_stats.vertices += cmd.indexCount;
    }

    m_drawQueue.clear();
    m_transforms.clear();

    if (m_currentFramebuffer) { m_currentFramebuffer->Unbind(); }
    m_currentFramebuffer = nullptr;
}

void RenderModule::SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform)
{
    // process all surfaces of the mesh and submit draw commands for them
    for (const auto& surf : mesh->GetSurfaces()) {
        const auto& material = Assets().GetAsset<Material>(surf.materialHandle);
        if (!material) {
            wrn("Could not get material for surface");
            return;
        }

        const auto& pipeline = m_pipelines.pbr; // all standard meshes are PBR for now

        m_transforms.push_back(transform * surf.transform);
        m_drawQueue.push_back(
            {
                .transformIndex = static_cast<u32>(m_transforms.size() - 1),
                .indexCount = surf.indexCount,
                .vertices = surf.vertices.get(),
                .indices = surf.indices.get(),
                .pipeline = pipeline.get(),
                .material = material.get(),
            }
        );
    }
}

const RenderStats& RenderModule::GetStats() const { return m_stats; }

Ref<GraphicsPipeline> RenderModule::GetPBRPipeline() const { return m_pipelines.pbr; }

void RenderModule::ReloadShaders() { m_shaderLibrary.ReloadShaders(); }

void RenderModule::BindMaterial(const Material* material, const Shader* shader)
{
    if (!material || !shader) {
        wrn("Cannot bind nullptr Material!");
        return;
    }

    // set pbr params
    shader->SetUniform("u_baseColorFactor", material->baseColor);
    shader->SetUniform("u_metallicFactor", material->metallic);
    shader->SetUniform("u_roughnessFactor", material->roughness);
    shader->SetUniform("u_emissionColor", material->emissive);
    shader->SetUniform("u_occlusionStrength", material->ambientOcclusion);
    shader->SetUniform("u_normalScale", material->normalScale);
    shader->SetUniform("u_alphaCutoff", material->alphaCutoff);

    u32 materialFlags = 0;

    // @formatter:off
    struct Item { Material::TextureRole role; const char* name; u32 slot; };
    static Vector<Item> items{
        { Material::TextureRole::BaseColor          , "u_baseColorMap"          , 0 },
        { Material::TextureRole::MetallicRoughness  , "u_metallicRoughnessMap"  , 1 },
        { Material::TextureRole::Emission           , "u_emissionMap"           , 2 },
        { Material::TextureRole::Occlusion          , "u_occlusionMap"          , 3 },
        { Material::TextureRole::Normal             , "u_normalMap"             , 4 },
    };
    // @formatter:on

    for (const auto& item : items) {
        const auto handle = material->getTexture(item.role);
        if (!handle) { continue; }
        const auto texture = Assets().GetAsset<Texture2D>(*handle);
        texture->Attach(item.slot);
        shader->SetUniformTexture(item.name, item.slot);
        materialFlags |= 1 << item.slot;
        m_stats.textureBinds++;
    }

    // skybox
    if (m_renderInfo.environmentInfo.skybox) {
        m_renderInfo.environmentInfo.skybox->Attach(15);
        shader->SetUniformTexture("u_skybox", 15); // reserved for skybox
        m_stats.textureBinds++;
        materialFlags |= 1 << 5;
    }

    shader->SetUniform("u_materialFlags", materialFlags);
}

void RenderModule::DrawSkyLight()
{
    if (!m_pipelines.skybox || !m_unitCube || !m_renderInfo.environmentInfo.skybox) { return; }

    m_pipelines.skybox->Bind();
    m_stats.pipelineBinds++;

    m_renderInfo.environmentInfo.skybox->Attach(0);
    m_pipelines.skybox->GetShader()->SetUniformTexture("u_skybox", 0);

    const auto view = glm::mat4(glm::mat3(m_renderInfo.cameraInfo.viewMatrix));
    m_pipelines.skybox->GetShader()->SetUniform("u_projectionView", m_renderInfo.cameraInfo.projectionMatrix * view);

    glVertexArrayVertexBuffer(
        m_pipelines.skybox->GetVertexArrayID(),
        0,
        m_unitCube->vertices->GetID(),
        0,
        m_pipelines.skybox->GetStride()
    );

    glVertexArrayElementBuffer(m_pipelines.skybox->GetVertexArrayID(), m_unitCube->indices->GetID());

    glDrawElements(GL_TRIANGLES, m_unitCube->indexCount, GL_UNSIGNED_INT, nullptr);
    m_stats.drawCalls++;
    m_stats.vertices += m_unitCube->indexCount;
}
} // namespace siren::core
