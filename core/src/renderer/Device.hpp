#pragma once

#include "utilities/spch.hpp"
#include "resources/Buffer.hpp"
#include "resources/FrameBuffer.hpp"
#include "resources/GraphicsPipeline.hpp"
#include "resources/Texture.hpp"
#include "shaders/Shader.hpp"
#include "CommandBuffer.hpp"


namespace siren::core
{

struct ImageDescriptor {
    ImageFormat format;
    ImageExtent extent;
    ImageDimension dimension;
    u32 mipmap_levels;
};

struct SamplerDescriptor {
    /// @brief Tells the gpu how to filter when the source image is smaller.
    ImageFilterMode min_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to filter when the source image is larger.
    ImageFilterMode max_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to filter between mipmap levels.
    ImageFilterMode mipmap_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to wrap along the horizontal axis.
    ImageWrapMode s_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu how to wrap along the vertical axis.
    ImageWrapMode t_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu how to wrap along the depth axis.
    ImageWrapMode r_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu the highest resolution mipmap it can use.
    f32 lod_min = 0.f;
    /// @brief Tells the gpu the lowest resolution mipmap it can use.
    f32 lod_max = 0.f;
    /// @brief A custom user specified color for the image border.
    std::optional<glm::vec4> border_color = std::nullopt;
    /// @brief Tells the gpu how to sample depth.
    ImageCompareMode compare_mode = ImageCompareMode::None;
    /// @brief The function with which to sample depth.
    ImageCompareFn compare_fn = ImageCompareFn::LessEqual;
};

/**
 * @brief Interface providing low level methods wrapping the
 * underlying Graphics API.
 */
class Device {
public:
    virtual ~Device() = default;

    /// @brief Creates and returns a new @ref Buffer given a @ref BufferDescriptor.
    virtual auto create_buffer(const BufferDescriptor& desc) -> Buffer = 0;
    /// @brief Queues the given @ref Buffer for deletion.
    virtual auto destroy_buffer(BufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    virtual auto create_image(const ImageDescriptor& desc) -> Image = 0;
    /// @brief Queues the given @ref Image for deletion.
    virtual auto destroy_image(ImageHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Image given an @ref ImageDescriptor.
    virtual auto create_sampler(const SamplerDescriptor& desc) -> Sampler = 0;
    /// @brief Queues the given @ref ImageSampler for deletion.
    virtual auto destroy_image(SamplerHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Framebuffer given a @ref FramebufferDescriptor.
    virtual auto create_framebuffer(const FrameBufferDescriptor& desc) -> FrameBuffer = 0;
    /// @brief Queues the given @ref Framebuffer for deletion.
    virtual auto destroy_framebuffer(FramebufferHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref Shader given a @ref ShaderDescriptor.
    virtual auto create_shader(const ShaderDescriptor& desc) -> Shader = 0;
    /// @brief Queues the given @ref Shader for deletion.
    virtual auto destroy_shader(ShaderHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref GraphicsPipeline given a @ref GraphicsPipelineDescriptor.
    virtual auto create_graphics_pipeline(const GraphicsPipelineDescriptor& desc) -> GraphicsPipeline = 0;
    /// @brief Queues the given @ref GraphicsPipeline for deletion.
    virtual auto destroy_graphics_pipeline(GraphicsPipelineHandle handle) -> void = 0;

    /// @brief Creates and returns a new @ref CommandBuffer. Provides an API for execution tasks.
    virtual auto record_commands() -> std::unique_ptr<CommandBuffer> = 0;
    /// @brief Submits a @ref CommandBuffer for execution.
    virtual auto submit(std::unique_ptr<CommandBuffer>&& cmd_buffer) -> void = 0;
};
} // namespace siren::core
