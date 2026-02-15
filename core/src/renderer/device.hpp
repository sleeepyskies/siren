#pragma once

#include "utilities/spch.hpp"

#include "resources/buffer.hpp"
#include "resources/framebuffer.hpp"
#include "resources/graphics_pipeline.hpp"
#include "resources/image.hpp"
#include "resources/shader.hpp"
#include "command_buffer.hpp"


namespace siren::core
{

/**
 * @brief The Device manages the lifetime of @ref RenderResource objects.
 * Furthermore, it is the primary entry point for all interactions with the GPU.
 * @note Many Device operations are performed asynchronously. In the case of
 * RenderResource creation, the siren abstraction is retuned immediately, but
 * the GPU object may not exist yet. However, the @ref RenderThread executes tasks
 * sequentially, and thus any requested operations on a GPU object should be fine.
 */
class Device {
public:
    virtual ~Device() = default;

    /// @brief Creates and returns a new @ref Buffer given a @ref BufferDescriptor.
    [[nodiscard]] virtual auto create_buffer(const BufferDescriptor& descriptor) -> Buffer = 0;
    /// @brief Queues the given @ref Buffer for deletion.
    virtual auto destroy_buffer(BufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    [[nodiscard]] virtual auto create_image(const ImageDescriptor& descriptor) -> Image = 0;
    /// @brief Queues the given @ref Image for deletion.
    virtual auto destroy_image(ImageHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    [[nodiscard]] virtual auto create_sampler(const SamplerDescriptor& descriptor) -> Sampler = 0;
    /// @brief Queues the given @ref ImageSampler for deletion.
    virtual auto destroy_sampler(SamplerHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Framebuffer given a @ref FramebufferDescriptor.
    [[nodiscard]] virtual auto create_framebuffer(const FramebufferDescriptor& descriptor) -> Framebuffer = 0;
    /// @brief Queues the given @ref Framebuffer for deletion.
    virtual auto destroy_framebuffer(FramebufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Shader given a @ref ShaderDescriptor.
    [[nodiscard]] virtual auto create_shader(const ShaderDescriptor& descriptor) -> Shader = 0;
    /// @brief Queues the given @ref Shader for deletion.
    virtual auto destroy_shader(ShaderHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref GraphicsPipeline given a @ref GraphicsPipelineDescriptor.
    [[nodiscard]] virtual auto create_graphics_pipeline(
        const GraphicsPipelineDescriptor& descriptor
    ) -> GraphicsPipeline = 0;
    /// @brief Queues the given @ref GraphicsPipeline for deletion.
    virtual auto destroy_graphics_pipeline(GraphicsPipelineHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref CommandBuffer. Provides an API for execution tasks.
    [[nodiscard]] virtual auto record_commands() -> std::unique_ptr<CommandBuffer> = 0;
    /// @brief Submits a @ref CommandBuffer for execution.
    virtual auto submit(std::unique_ptr<CommandBuffer>&& command_buffer) -> void = 0;
    /// @brief Flushes the delete queue of the device. Should only be called at the end of a frame.
    virtual auto flush_delete_queue() -> void = 0;
};

} // namespace siren::core

