#pragma once

#include "renderer/device.hpp"


namespace siren::platform
{

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

    [[nodiscard]] auto record_commands() -> std::unique_ptr<core::CommandBuffer> override;
    auto submit(std::unique_ptr<core::CommandBuffer>&& command_buffer) -> void override;

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

    std::shared_ptr<spdlog::logger> m_logger;

    template <typename Resource>
    using GlResourceTable = core::RenderResourceTable<GLuint, Resource>;

    /// @brief All objects queued for cleanup.
    std::vector<DeleteRequest> m_delete_queue;

    /// @brief Buffer handle storage.
    core::RenderResourceTable<GLuint, core::Buffer, void*> m_buffer_table;
    /// @brief Image handle storage.
    GlResourceTable<core::Image> m_image_table;
    /// @brief Sampler handle storage.
    GlResourceTable<core::Sampler> m_sampler_table;
    /// @brief Framebuffer handle storage.
    GlResourceTable<core::Framebuffer> m_framebuffer_table;
    /// @brief Shader handle storage.
    core::RenderResourceTable<GLuint, core::Shader, std::flat_map<std::string, GLint>> m_shader_table;
    /// @brief GraphicsPipeline handle storage.
    /// @note The GLuint stored here is not of the Pipeline, but rather the vertex array.
    ///       This is because OpenGL has no notion of a Pipeline, but we use a VA in the pipeline.
    GlResourceTable<core::GraphicsPipeline> m_graphics_pipeline_table;
};

} // namespace siren::platform
