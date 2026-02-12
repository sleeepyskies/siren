#pragma once

#include "resources/buffer.hpp"
#include "pbr_material.hpp"
#include "resources/Framebuffer.hpp"
#include "resources/graphics_pipeline.hpp"
#include "render_info.hpp"

#include "geometry/mesh.hpp"
#include "geometry/primitive.hpp"

#include "renderer/shaders/shader.hpp"

#include "shaders/shader_library.hpp"


namespace siren::core
{
/**
 * @brief Render statistics for a single frame.
 */
struct RenderStats {
    u32 draw_calls     = 0;
    u32 vertices       = 0;
    u32 pipeline_binds = 0;
    u32 texture_binds  = 0;

    void reset() {
        draw_calls     = 0;
        vertices       = 0;
        pipeline_binds = 0;
        texture_binds  = 0;
    }
};

struct alignas(16) LightUBO {
    std::array<GPUPointLight, MAX_LIGHT_COUNT> point_lights;
    std::array<GPUDirectionalLight, MAX_LIGHT_COUNT> directional_lights;
    std::array<GPUSpotLight, MAX_LIGHT_COUNT> spot_lights;
    u32 point_light_count;
    u32 directional_light_count;
    u32 spot_light_count;
    u32 _pad;
};

// static_assert(sizeof(LightUBO) == 16 * 32 * 3 + 4 * 4);

struct alignas(16) CameraUBO {
    glm::mat4 projection_view;
    glm::vec3 camera_position;
    float _pad;
};

static_assert(sizeof(CameraUBO) == 4 * 16 + 4 * 3 + 4);

/**
 * @brief The RenderModule is the 3D renderer of Siren. Responsible for submitting draw calls and managing render state.
 * @todo Make the RenderModule API agnostic!
 * @todo Make a static class? namespaced functions?
 */
class Renderer {
public:
    bool init();
    void shutdown();

    /// @brief Starts a new frame with the given @ref RenderInfo.
    void begin_frame(const RenderInfo& renderInfo);
    /// @brief Ends the current frame.
    void end_frame();

    /// @brief Begins a render pass. Optionally takes a @ref FrameBuffer and a clear color.
    void begin_pass(
        const std::shared_ptr<Framebuffer>& frameBuffer,
        const glm::vec4& clearColor = { 0, 0, 0, 1 }
    );
    /// @brief Ends a render pass. (GPU talk happens here)
    void end_pass();

    /// @brief Submits a mesh.
    void submit_mesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform);

    /// @brief Return a reference to the current @ref RenderStats.
    const RenderStats& stats() const;
    /// @brief Returns the PBR pipeline.
    std::shared_ptr<GraphicsPipeline> pbr_pipeline() const;
    /// @brief Reloads all core shaders.
    void reload_shaders();

private:
    void bind_material(const PBRMaterial* material, const Shader* shader);
    void draw_sky_light();

    struct // container for pipelines
    {
        std::shared_ptr<GraphicsPipeline> pbr;
        std::shared_ptr<GraphicsPipeline> skybox;
        // std::shared_ptr<GraphicsPipeline> wireframe;
        // std::shared_ptr<GraphicsPipeline> unlit;
    } m_pipelines;

    std::shared_ptr<PrimitiveMeshData> m_unit_cube;

    RenderStats m_stats{ };
    RenderInfo m_render_info{ };

    ShaderLibrary m_shader_library;

    Framebuffer* m_current_framebuffer = nullptr;

    std::unique_ptr<Buffer> m_camera_buffer = nullptr; //< Bound to slot 0 always
    std::unique_ptr<Buffer> m_light_buffer  = nullptr; //< Bound to slot 1 always

    /// @brief Struct containing a single draw command. Used for batching draw calls at the end of a frame.
    struct DrawCommand {
        u32 transform_index;
        u32 index_count;
        Buffer* vertices;
        Buffer* indices;
        GraphicsPipeline* pipeline;
        PBRMaterial* material;

        explicit operator bool() const { return index_count > 0 && vertices && indices && pipeline && material; }
    };

    std::vector<DrawCommand> m_draw_queue{ };
    std::vector<glm::mat4> m_transforms{ };
};
} // namespace siren::core
