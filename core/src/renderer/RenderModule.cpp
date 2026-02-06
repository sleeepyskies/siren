// ReSharper disable CppVariableCanBeMadeConstexpr
// ReSharper disable CppMemberFunctionMayBeConst
#define GLM_ENABLE_EXPERIMENTAL
#include "RenderModule.hpp"

#include "shaders/Shader.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/ShaderUtils.hpp"

#include "window/WindowModule.hpp"


namespace siren::core
{
bool Renderer::Init() {
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_cameraBuffer = create_own<Buffer>(nullptr, sizeof(CameraUBO), BufferUsage::Dynamic);
    m_lightBuffer  = create_own<Buffer>(nullptr, sizeof(LightUBO), BufferUsage::Dynamic);

    // load shaders
    {
        m_shaderLibrary.Import("ass://shaders/pbr.sshg", "PBR");
        m_shaderLibrary.Import("ass://shaders/grid.sshg", "Grid");
        m_shaderLibrary.Import("ass://shaders/skyLight.sshg", "SkyBox");
    }

    // pbr pipeline
    {
        GraphicsPipeline::Properties props;
        props.layout.set_layout(
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
        m_pipelines.pbr       = create_ref<GraphicsPipeline>(props, "PBR Pipeline");
    }

    // skybox pipeline
    {
        GraphicsPipeline::Properties props;
        props.layout.set_layout({ VertexAttribute::Position });
        props.topology        = PrimitiveTopology::Triangles;
        props.alphaMode       = AlphaMode::Opaque;
        props.depthFunction   = DepthFunction::LessEqual;
        props.backFaceCulling = false;
        props.depthTest       = false;
        props.depthWrite      = false;
        props.shader          = m_shaderLibrary.Get("SkyBox");
        m_pipelines.skybox    = create_ref<GraphicsPipeline>(props, "SkyBox Pipeline");
    }

    m_unitCube = primitive::Generate(CubeParams{ }, m_pipelines.skybox->get_layout());

    return true;
}

void Renderer::Shutdown() {
    // nothing for now
}

void Renderer::BeginFrame(const RenderInfo& renderInfo) {
    m_stats.Reset();
    m_renderInfo = renderInfo;

    CameraUBO cameraUbo;
    cameraUbo.projectionView = renderInfo.cameraInfo.projectionMatrix * renderInfo.cameraInfo.viewMatrix;
    cameraUbo.cameraPosition = renderInfo.cameraInfo.position;
    m_cameraBuffer->update(&cameraUbo, sizeof(CameraUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer->id());

    LightUBO lightUbo;
    lightUbo.pointLights           = renderInfo.lightInfo.pointLights;
    lightUbo.directionalLights     = renderInfo.lightInfo.directionalLights;
    lightUbo.spotLights            = renderInfo.lightInfo.spotLights;
    lightUbo.pointLightCount       = renderInfo.lightInfo.pointLightCount;
    lightUbo.directionalLightCount = renderInfo.lightInfo.directionalLightCount;
    lightUbo.spotLightCount        = renderInfo.lightInfo.spotLightCount;

    m_lightBuffer->update(&lightUbo, sizeof(LightUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer->id());
}

void Renderer::EndFrame() {
    // todo: we should really add a SubmitSkybox fn, but that requires a more complex BindMaterial()
}

void Renderer::BeginPass(const Ref<FrameBuffer>& frameBuffer, const glm::vec4& clearColor) {
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

void Renderer::EndPass() {
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
    const PBRMaterial* lastMaterial      = nullptr;

    for (const auto& cmd : m_drawQueue) {
        if (!cmd) { continue; }

        if (cmd.pipeline != lastPipeline) {
            cmd.pipeline->bind();
            lastPipeline = cmd.pipeline;
            m_stats.pipelineBinds++;
        }

        if (cmd.material != lastMaterial) {
            BindMaterial(cmd.material, cmd.pipeline->get_shader().get());
            lastMaterial = cmd.material;
        }

        cmd.pipeline->get_shader()->SetUniform("u_model", m_transforms[cmd.transformIndex]);

        const GLenum top = topologyToGlEnum(cmd.pipeline->get_topology());

        if (cmd.vertices != lastVertices) {
            glVertexArrayVertexBuffer(
                cmd.pipeline->get_vertex_array_id(),
                0,
                cmd.vertices->id(),
                0,
                cmd.pipeline->get_stride()
            );
            lastVertices = cmd.vertices;
        }

        if (cmd.indices != lastIndices) {
            glVertexArrayElementBuffer(cmd.pipeline->get_vertex_array_id(), cmd.indices->id());
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

void Renderer::SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform) {
    // process all surfaces of the mesh and submit draw commands for them
    for (const auto& surf : mesh->get_surfaces()) {
        const auto& material = Assets().GetAsset<PBRMaterial>(surf.material_handle);
        if (!material) {
            wrn("Could not get material for surface");
            return;
        }

        const auto& pipeline = m_pipelines.pbr; // all standard meshes are PBR for now

        m_transforms.push_back(transform * surf.transform);
        m_drawQueue.push_back(
            {
                .transformIndex = static_cast<u32>(m_transforms.size() - 1),
                .indexCount = surf.idx_count,
                .vertices = surf.vertices.get(),
                .indices = surf.indices.get(),
                .pipeline = pipeline.get(),
                .material = material.get(),
            }
        );
    }
}

const RenderStats& Renderer::GetStats() const { return m_stats; }

Ref<GraphicsPipeline> Renderer::GetPBRPipeline() const { return m_pipelines.pbr; }

void Renderer::ReloadShaders() { m_shaderLibrary.ReloadShaders(); }

void Renderer::BindMaterial(const PBRMaterial* material, const Shader* shader) {
    if (!material || !shader) {
        wrn("Cannot bind nullptr Material!");
        return;
    }

    // set pbr params
    shader->set_uniform("u_baseColorFactor", material->baseColor);
    shader->SetUniform("u_metallicFactor", material->metallic);
    shader->SetUniform("u_roughnessFactor", material->roughness);
    shader->set_uniform("u_emissionColor", material->emissive);
    shader->set_uniform("u_occlusionStrength", material->ambientOcclusion);
    shader->set_uniform("u_normalScale", material->normalScale);
    shader->set_uniform("u_alphaCutoff", material->alphaCutoff);

    u32 materialFlags = 0;

    // @formatter:off
    struct Item { PBRMaterial::TextureRole role; const char* name; u32 slot; };
    static Vector<Item> items{
        { PBRMaterial::TextureRole::BaseColor          , "u_baseColorMap"          , 0 },
        { PBRMaterial::TextureRole::MetallicRoughness  , "u_metallicRoughnessMap"  , 1 },
        { PBRMaterial::TextureRole::Emission           , "u_emissionMap"           , 2 },
        { PBRMaterial::TextureRole::Occlusion          , "u_occlusionMap"          , 3 },
        { PBRMaterial::TextureRole::Normal             , "u_normalMap"             , 4 },
    };
    // @formatter:on

    for (const auto& item : items) {
        const auto handle = material->getTexture(item.role);
        if (!handle) { continue; }
        const auto texture = Assets().GetAsset<Texture2D>(*handle);
        texture->Attach(item.slot);
        shader->set_uniform_texture(item.name, item.slot);
        materialFlags |= 1 << item.slot;
        m_stats.textureBinds++;
    }

    // skybox
    if (m_renderInfo.environmentInfo.skybox) {
        m_renderInfo.environmentInfo.skybox->attach(15);
        shader->set_uniform_texture("u_skybox", 15); // reserved for skybox
        m_stats.textureBinds++;
        materialFlags |= 1 << 5;
    }

    shader->set_uniform("u_materialFlags", materialFlags);
}

void Renderer::DrawSkyLight() {
    if (!m_pipelines.skybox || !m_unitCube || !m_renderInfo.environmentInfo.skybox) { return; }

    m_pipelines.skybox->bind();
    m_stats.pipelineBinds++;

    m_renderInfo.environmentInfo.skybox->attach(0);
    m_pipelines.skybox->get_shader()->SetUniformTexture("u_skybox", 0);

    const auto view = glm::mat4(glm::mat3(m_renderInfo.cameraInfo.viewMatrix));
    m_pipelines.skybox->get_shader()->SetUniform("u_projectionView", m_renderInfo.cameraInfo.projectionMatrix * view);

    glVertexArrayVertexBuffer(
        m_pipelines.skybox->get_vertex_array_id(),
        0,
        m_unitCube->vertices->id(),
        0,
        m_pipelines.skybox->get_stride()
    );

    glVertexArrayElementBuffer(m_pipelines.skybox->get_vertex_array_id(), m_unitCube->indices->id());

    glDrawElements(GL_TRIANGLES, m_unitCube->indexCount, GL_UNSIGNED_INT, nullptr);
    m_stats.drawCalls++;
    m_stats.vertices += m_unitCube->indexCount;
}
} // namespace siren::core
