// ReSharper disable CppVariableCanBeMadeConstexpr
// ReSharper disable CppMemberFunctionMayBeConst
#define GLM_ENABLE_EXPERIMENTAL
#include "Renderer.hpp"

#include "shaders/Shader.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assets/AssetServer.hpp"

#include "shaders/ShaderUtils.hpp"

#include "window/WindowModule.hpp"


namespace siren::core
{
bool Renderer::init() {
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_camera_buffer = std::make_unique<Buffer>(nullptr, sizeof(CameraUBO), BufferUsage::Dynamic);
    m_light_buffer  = std::make_unique<Buffer>(nullptr, sizeof(LightUBO), BufferUsage::Dynamic);

    // load shaders
    {
        m_shader_library.import_shader("ass://shaders/pbr.sshg", "PBR");
        m_shader_library.import_shader("ass://shaders/grid.sshg", "Grid");
        m_shader_library.import_shader("ass://shaders/skyLight.sshg", "SkyBox");
    }

    // pbr pipeline
    {
        GraphicsPipeline::Description props;
        props.layout.set_layout(
            {
                VertexAttribute::Position,
                VertexAttribute::Normal,
                VertexAttribute::Tangent,
                VertexAttribute::Bitangent,
                VertexAttribute::Texture
            }
        );
        props.topology          = PrimitiveTopology::Triangles;
        props.alpha_mode        = AlphaMode::Opaque;
        props.depth_function    = DepthFunction::Less;
        props.back_face_culling = true;
        props.depth_test        = true;
        props.depth_write       = true;
        props.shader            = m_shader_library.get("PBR");
        m_pipelines.pbr         = create_ref<GraphicsPipeline>(props, "PBR Pipeline");
    }

    // skybox pipeline
    {
        GraphicsPipeline::Description props;
        props.layout.set_layout({ VertexAttribute::Position });
        props.topology          = PrimitiveTopology::Triangles;
        props.alpha_mode        = AlphaMode::Opaque;
        props.depth_function    = DepthFunction::LessEqual;
        props.back_face_culling = false;
        props.depth_test        = false;
        props.depth_write       = false;
        props.shader            = m_shader_library.get("SkyBox");
        m_pipelines.skybox      = create_ref<GraphicsPipeline>(props, "SkyBox Pipeline");
    }

    m_unit_cube = primitive::generate(CubeParams{ }, m_pipelines.skybox->get_layout());

    return true;
}

void Renderer::shutdown() {
    // nothing for now
}

void Renderer::begin_frame(const RenderInfo& renderInfo) {
    m_stats.reset();
    m_render_info = renderInfo;

    CameraUBO cameraUbo;
    cameraUbo.projection_view = renderInfo.cameraInfo.projectionMatrix * renderInfo.cameraInfo.viewMatrix;
    cameraUbo.camera_position = renderInfo.cameraInfo.position;
    m_camera_buffer->update(&cameraUbo, sizeof(CameraUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera_buffer->id());

    LightUBO lightUbo;
    lightUbo.point_lights            = renderInfo.lightInfo.pointLights;
    lightUbo.directional_lights      = renderInfo.lightInfo.directionalLights;
    lightUbo.spot_lights             = renderInfo.lightInfo.spotLights;
    lightUbo.point_light_count       = renderInfo.lightInfo.pointLightCount;
    lightUbo.directional_light_count = renderInfo.lightInfo.directionalLightCount;
    lightUbo.spot_light_count        = renderInfo.lightInfo.spotLightCount;

    m_light_buffer->update(&lightUbo, sizeof(LightUBO));
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_light_buffer->id());
}

void Renderer::end_frame() {
    // todo: we should really add a SubmitSkybox fn, but that requires a more complex BindMaterial()
}

void Renderer::begin_pass(const std::shared_ptr<FrameBuffer>& frameBuffer, const glm::vec4& clearColor) {
    m_current_framebuffer = frameBuffer.get();

    if (m_current_framebuffer) {
        frameBuffer->Bind();
        frameBuffer->SetViewport();
    } else {
        const auto size = window().GetSize();
        glViewport(0, 0, size.x, size.y);
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::end_pass() {
    // todo: sort based on depth here too!
    std::sort(
        m_draw_queue.begin(),
        m_draw_queue.end(),
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
    draw_sky_light();

    const Buffer* lastVertices           = nullptr;
    const Buffer* lastIndices            = nullptr;
    const GraphicsPipeline* lastPipeline = nullptr;
    const PBRMaterial* lastMaterial      = nullptr;

    for (const auto& cmd : m_draw_queue) {
        if (!cmd) { continue; }

        if (cmd.pipeline != lastPipeline) {
            cmd.pipeline->bind();
            lastPipeline = cmd.pipeline;
            m_stats.pipeline_binds++;
        }

        if (cmd.material != lastMaterial) {
            bind_material(cmd.material, cmd.pipeline->get_shader().get());
            lastMaterial = cmd.material;
        }

        cmd.pipeline->get_shader()->SetUniform("u_model", m_transforms[cmd.transform_index]);

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

        glDrawElements(top, cmd.index_count, GL_UNSIGNED_INT, nullptr);
        m_stats.draw_calls++;
        m_stats.vertices += cmd.index_count;
    }

    m_draw_queue.clear();
    m_transforms.clear();

    if (m_current_framebuffer) { m_current_framebuffer->unbind(); }
    m_current_framebuffer = nullptr;
}

void Renderer::submit_mesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform) {
    auto& asset_server = Locator<AssetServer>::locate();

    // process all surfaces of the mesh and submit draw commands for them
    for (const auto& surface_handle : mesh->surfaces) {
        const auto* surface  = asset_server.get<Surface>(surface_handle);
        const auto* material = asset_server.get<PBRMaterial>(surface->material);

        if (!material) {
            wrn("Could not get material for surface");
            return;
        }

        const auto& pipeline = m_pipelines.pbr; // all standard meshes are PBR for now

        m_transforms.push_back(transform * surface_handle.transform);
        m_draw_queue.push_back(
            {
                .transform_index = static_cast<u32>(m_transforms.size() - 1),
                .index_count = surface_handle.idx_count,
                .vertices = surface_handle.vertices.get(),
                .indices = surface_handle.indices.get(),
                .pipeline = pipeline.get(),
                .material = material.get(),
            }
        );
    }
}

const RenderStats& Renderer::stats() const { return m_stats; }

std::shared_ptr<GraphicsPipeline> Renderer::pbr_pipeline() const { return m_pipelines.pbr; }

void Renderer::reload_shaders() { m_shader_library.reload_shaders(); }

void Renderer::bind_material(const PBRMaterial* material, const Shader* shader) {
    if (!material || !shader) {
        wrn("Cannot bind nullptr Material!");
        return;
    }

    // set pbr params
    shader->set_uniform("u_baseColorFactor", material->baseColor);
    shader->set_uniform("u_metallicFactor", material->metallic);
    shader->set_uniform("u_roughnessFactor", material->roughness);
    shader->set_uniform("u_emissionColor", material->emissive);
    shader->set_uniform("u_occlusionStrength", material->ambientOcclusion);
    shader->set_uniform("u_normalScale", material->normalScale);
    shader->set_uniform("u_alphaCutoff", material->alphaCutoff);

    u32 materialFlags = 0;

    // @formatter:off
    struct Item { PBRMaterial::TextureRole role; const char* name; u32 slot; };
    static std::vector<Item> items{
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
        m_stats.texture_binds++;
    }

    // skybox
    if (m_render_info.environmentInfo.skybox) {
        m_render_info.environmentInfo.skybox->attach(15);
        shader->set_uniform_texture("u_skybox", 15); // reserved for skybox
        m_stats.texture_binds++;
        materialFlags |= 1 << 5;
    }

    shader->set_uniform("u_materialFlags", materialFlags);
}

void Renderer::draw_sky_light() {
    if (!m_pipelines.skybox || !m_unit_cube || !m_render_info.environmentInfo.skybox) { return; }

    m_pipelines.skybox->bind();
    m_stats.pipeline_binds++;

    m_render_info.environmentInfo.skybox->attach(0);
    m_pipelines.skybox->get_shader()->SetUniformTexture("u_skybox", 0);

    const auto view = glm::mat4(glm::mat3(m_render_info.cameraInfo.viewMatrix));
    m_pipelines.skybox->get_shader()->SetUniform("u_projectionView", m_render_info.cameraInfo.projectionMatrix * view);

    glVertexArrayVertexBuffer(
        m_pipelines.skybox->get_vertex_array_id(),
        0,
        m_unit_cube->vertices->id(),
        0,
        m_pipelines.skybox->get_stride()
    );

    glVertexArrayElementBuffer(m_pipelines.skybox->get_vertex_array_id(), m_unit_cube->indices->id());

    glDrawElements(GL_TRIANGLES, m_unit_cube->indexCount, GL_UNSIGNED_INT, nullptr);
    m_stats.draw_calls++;
    m_stats.vertices += m_unit_cube->indexCount;
}
} // namespace siren::core
