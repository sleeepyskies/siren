#pragma once

#include "renderer/device.hpp"


namespace siren::platform
{

/**
 * @brief Encapsulates a mapped buffer pointer. This is used in streamed @ref Buffer's.
 */
struct MappedBufferPtr {
    /// @brief The mapped pointer.
    void* ptr = nullptr;
    /// @brief The size of the buffer.
    usize size = 0;
};

struct OpenGLBufferDetails {
    core::BufferDescriptor descriptor;
    MappedBufferPtr buffer_ptr;
};

struct OpenGLImageDetails {
    core::ImageDescriptor descriptor;
};

struct OpenGLSamplerDetails {
    core::SamplerDescriptor descriptor;
};

struct OpenGLFramebufferDetails {
    core::FramebufferDescriptor descriptor;
};

struct OpenGLShaderDetails {
    core::ShaderDescriptor descriptor;
    std::flat_map<std::string, GLint> uniform_cache;
};

struct OpenGLGraphicsPipelineDetails {
    core::GraphicsPipelineDescriptor descriptor;
};

/**
 * @struct OpenGLRenderResourceState
 * @brief Encapsulates all @ref RenderResource state for the OpenGL backend.
 */

struct OpenGLRenderResourceState {
    /// @brief Buffer handle storage.
    core::RenderResourceTable<GLuint, core::Buffer, OpenGLBufferDetails> buffer_table;
    /// @brief Image handle storage.
    core::RenderResourceTable<GLuint, core::Image, OpenGLImageDetails> image_table;
    /// @brief Sampler handle storage.
    core::RenderResourceTable<GLuint, core::Sampler, OpenGLSamplerDetails> sampler_table;
    /// @brief Framebuffer handle storage.
    core::RenderResourceTable<GLuint, core::Framebuffer, OpenGLFramebufferDetails> framebuffer_table;
    /// @brief Shader handle storage.
    core::RenderResourceTable<GLuint, core::Shader, OpenGLShaderDetails> shader_table;
    /// @brief GraphicsPipeline handle storage.
    /// @note The GLuint stored here is not of the Pipeline, but rather the vertex array.
    ///       This is because OpenGL has no notion of a Pipeline, but we use a VA in the pipeline.
    core::RenderResourceTable<GLuint, core::GraphicsPipeline, OpenGLGraphicsPipelineDetails> graphics_pipeline_table;
};

class OpenGLDevice final : public core::Device {
public:
    OpenGLDevice();
    ~OpenGLDevice() override;

    [[nodiscard]] auto create_buffer(const core::BufferDescriptor& descriptor) -> core::Buffer override;
    auto destroy_buffer(core::BufferHandle handle) -> void override;

    [[nodiscard]] auto create_image(const core::ImageDescriptor& descriptor) -> core::Image override;
    auto destroy_image(core::ImageHandle handle) -> void override;

    [[nodiscard]] auto create_sampler(const core::SamplerDescriptor& descriptor) -> core::Sampler override;
    auto destroy_sampler(core::SamplerHandle handle) -> void override;

    [[nodiscard]] auto create_framebuffer(const core::FramebufferDescriptor& descriptor) -> core::Framebuffer override;
    auto destroy_framebuffer(core::FramebufferHandle handle) -> void override;

    [[nodiscard]] auto create_shader(const core::ShaderDescriptor& descriptor) -> core::Shader override;
    auto destroy_shader(core::ShaderHandle handle) -> void override;

    [[nodiscard]] auto create_graphics_pipeline(
        const core::GraphicsPipelineDescriptor& descriptor
    ) -> core::GraphicsPipeline override;
    auto destroy_graphics_pipeline(core::GraphicsPipelineHandle handle) -> void override;

    auto flush_delete_queue() -> void override;

    [[nodiscard]] auto record_resource_commands() -> core::ResourceCommandRecorder override;
    [[nodiscard]] auto record_render_commands() -> core::RenderCommandRecorder override;
    auto submit(core::ResourceCommandBuffer&& command_buffer) -> void override;
    auto submit(core::RenderCommandBuffer&& command_buffer) -> void override;

    [[nodiscard]] auto buffer_descriptor(core::BufferHandle handle) const -> const core::BufferDescriptor& override;
    [[nodiscard]] auto image_descriptor(core::ImageHandle handle) const -> const core::ImageDescriptor& override;
    [[nodiscard]] auto sampler_descriptor(core::SamplerHandle handle) const -> const core::SamplerDescriptor& override;
    [[nodiscard]] auto framebuffer_descriptor(
        core::FramebufferHandle handle
    ) const -> const core::FramebufferDescriptor& override;
    [[nodiscard]] auto shader_descriptor(core::ShaderHandle handle) const -> const core::ShaderDescriptor& override;
    [[nodiscard]] auto graphics_pipeline_descriptor(
        core::GraphicsPipelineHandle handle
    ) const -> const core::GraphicsPipelineDescriptor& override;

    [[nodiscard]] auto limits() const -> core::Limits override;

private:
    /**
     * @brief Enum listing all OpenGL GPU objects used.
     * @note We do not define a global enum for this, as different backends
     * may have a different way of doing things and not use the same GPU
     * objects.
     */
    enum class OpenGlResourceType {
        /// @brief A @ref Buffer.
        Buffer,
        /// @brief An @ref Image.
        Image,
        /// @brief A @ref Sampler.
        Sampler,
        /// @brief A @ref Framebuffer.
        Framebuffer,
        /// @brief A @ref Shader.
        Shader,
        /// @brief A @ref GraphicsPipeline.
        GraphicsPipeline,
    };

    /// @brief Describes a Delete that has been requested of a GPU object.
    struct DeleteRequest {
        /// @brief The native OpenGL object handle.
        GLuint handle;
        /// @brief The resource type of the object to be deleted.
        OpenGlResourceType type;
    };

    /// @brief Renderer Logger.
    std::shared_ptr<spdlog::logger> m_logger;

    /// @brief The state of @ref RenderResource's.
    OpenGLRenderResourceState m_state;

    /// @brief All objects queued for cleanup.
    std::vector<DeleteRequest> m_delete_queue;
};

} // namespace siren::platform
