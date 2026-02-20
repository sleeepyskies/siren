#pragma once

#include "opengl_device.hpp"
#include "renderer/command_executor.hpp"


namespace siren::platform
{

/**
 * @class OpenGLCommandExecutor
 * @brief The OpenGL specific @ref CommandExecutor.
 */
class OpenGLCommandExecutor final : public core::CommandExecutor {
public:
    explicit OpenGLCommandExecutor(const OpenGLRenderResourceState& state);
    ~OpenGLCommandExecutor() override = default;

    auto execute_resource_commands(core::ResourceCommandBuffer&& resource_command_pacakge) -> void override;
    auto execute_render_commands(core::RenderCommandBuffer&& render_command_package) -> void override;

private:
    const OpenGLRenderResourceState& m_state;

    /// @brief Handles @ref UploadImage.
    auto execute_image_upload(const core::UploadImage& cmd, std::span<const u8> data_slice) const -> void;
    /// @brief Handles @ref UploadBuffer.
    auto execute_buffer_upload(const core::UploadBuffer& cmd, std::span<const u8> data_slice) const -> void;

    /// @brief Executes a single @ref RenderPass.
    auto execute_pass(
        const core::RenderPassDescriptor& descriptor,
        std::span<const core::RenderCommand> commands
    ) const -> void;

    auto bind_graphics_pipeline(const core::BindGraphicsPipeline& bind) const -> void;
};

} // namespace siren::platform
