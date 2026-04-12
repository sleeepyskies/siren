module;

#include <glad/gl.h>

export module siren.render.opengl.opengl_command_executor;

import siren.render.graphics_pipeline;
import siren.render.command_executor;

namespace siren::render::opengl {

/**
 * @brief Struct for the OpenGL backend tracking any
 * state by OpenGL needed for the @ref Executor.
 */
struct TrackedState {
    GLuint active_vao                        = 0;
    GraphicsPipeline::Handle active_pipeline = GraphicsPipeline::Handle::invalid();
    BindIndexBuffer active_ibo               = {
        .index_buffer = core::BufferHandle::invalid(),
        .index_format = core::IndexFormat::Uint32
    };
};

/**
 * @class OpenGLCommandExecutor
 * @brief The OpenGL specific @ref CommandExecutor.
 */
class OpenGLCommandExecutor final : public CommandExecutor {
public:
    explicit OpenGLCommandExecutor(const OpenGLRenderResourceState& state);
    ~OpenGLCommandExecutor() override = default;

    /**
     * @brief Executes the provided @ref ResourceCommand's
     * @param resource_command_pacakge The commands to execute.
     * @note This function should only be called from within a RenderThread::spawn lambda!!!!
     */
    auto execute_resource_commands(core::ResourceCommandBuffer&& resource_command_pacakge) -> void override;

    /**
     * @brief Executes the provided @ref RenderCommands's
     * @param render_command_package The commands to execute.
     * @note This function should only be called from within a RenderThread::spawn lambda!!!!
     */
    auto execute_render_commands(core::RenderCommandBuffer&& render_command_package) -> void override;

private:
    const OpenGLRenderResourceState& m_state;
    mutable TrackedState m_tracked_state;

    /// @brief Handles @ref UploadImage.
    auto execute_image_upload(const core::UploadImage& cmd, std::span<const u8> data_slice) const -> void;
    /// @brief Handles @ref UploadBuffer.
    auto execute_buffer_upload(const core::UploadBuffer& cmd, std::span<const u8> data_slice) const -> void;

    /// @brief Executes a single @ref RenderPass.
    auto execute_pass(
        const core::RenderPassDescriptor& descriptor,
        std::span<const core::RenderCommand> commands
    ) const -> void;

    /// @brief Handles @ref BindGraphicsPipeline.
    auto bind_graphics_pipeline(const core::BindGraphicsPipeline& bind) const -> void;
    /// @brief Handles @ref SetViewport.
    auto set_viewport(const core::SetViewport& set_viewport, core::FramebufferHandle fb_handle) const -> void;
    /// @brief Handles @ref BindVertexBuffer.
    auto bind_vertex_buffer(const core::BindVertexBuffer& bind_vertex_buffer) const -> void;
    /// @brief Handles @ref BindIndexBuffer.
    auto bind_index_buffer(const core::BindIndexBuffer& bind_index_buffer) const -> void;
    /// @brief Handles @ref BindUniformBuffer.
    auto bind_uniform_buffer(const core::BindUniformBuffer& bind_uniform_buffer) const -> void;
    /// @brief Handles @ref DrawArrays.
    auto draw_arrays(const core::DrawArrays& draw_arrays) const -> void;
    /// @brief Handles @ref DrawIndexed.
    auto draw_indexed(const core::DrawIndexed& draw_indexed) const -> void;
};

} // namespace siren::render::opengl
