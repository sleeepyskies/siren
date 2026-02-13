#pragma once

#include "utilities/spch.hpp"
#include "resources/buffer.hpp"
#include "resources/Framebuffer.hpp"
#include "resources/graphics_pipeline.hpp"
#include "resources/texture.hpp"
#include "shaders/shader.hpp"
#include "command_buffer.hpp"


namespace siren::core
{

/**
 * @brief Interface providing low level methods wrapping the
 * underlying Graphics API.
 */
class Device {
public:
    virtual ~Device() = default;

    /// @brief Creates and returns a new @ref Buffer given a @ref BufferDescriptor.
    virtual auto create_buffer(const BufferDescriptor& descriptor) -> Buffer = 0;
    /// @brief Queues the given @ref Buffer for deletion.
    virtual auto destroy_buffer(BufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    virtual auto create_image(const ImageDescriptor& descriptor) -> Image = 0;
    /// @brief Queues the given @ref Image for deletion.
    virtual auto destroy_image(ImageHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    virtual auto create_sampler(const SamplerDescriptor& descriptor) -> Sampler = 0;
    /// @brief Queues the given @ref ImageSampler for deletion.
    virtual auto destroy_sampler(SamplerHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Framebuffer given a @ref FramebufferDescriptor.
    virtual auto create_framebuffer(const FrameBufferDescriptor& descriptor) -> Framebuffer = 0;
    /// @brief Queues the given @ref Framebuffer for deletion.
    virtual auto destroy_framebuffer(FramebufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Shader given a @ref ShaderDescriptor.
    virtual auto create_shader(const ShaderDescriptor& descriptor) -> Shader = 0;
    /// @brief Queues the given @ref Shader for deletion.
    virtual auto destroy_shader(ShaderHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref GraphicsPipeline given a @ref GraphicsPipelineDescriptor.
    virtual auto create_graphics_pipeline(const GraphicsPipelineDescriptor& descriptor) -> GraphicsPipeline = 0;
    /// @brief Queues the given @ref GraphicsPipeline for deletion.
    virtual auto destroy_graphics_pipeline(GraphicsPipelineHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref CommandBuffer. Provides an API for execution tasks.
    virtual auto record_commands() -> std::unique_ptr<CommandBuffer> = 0;
    /// @brief Submits a @ref CommandBuffer for execution.
    virtual auto submit(std::unique_ptr<CommandBuffer>&& command_buffer) -> void = 0;
};
} // namespace siren::core
