#pragma once

#include "renderer/device.hpp"


namespace siren::platform
{

enum class OpenGlResourceType {
    Buffer,
    Image,
    Sampler,
    Framebuffer,
    Shader,
    GraphicsPipeline,
};

struct DeleteRequest {
    GLuint handle;
    OpenGlResourceType type;
};

class OpenGlDevice final : public core::Device {
public:
    OpenGlDevice();
    ~OpenGlDevice() override;

    auto create_buffer(const core::BufferDescriptor& desc) -> core::Buffer override;
    auto destroy_buffer(core::BufferHandle handle) -> void override;

    auto create_image(const core::ImageDescriptor& desc) -> core::Image override;
    auto destroy_image(core::ImageHandle handle) -> void override;

private:
    template <typename Resource>
    using GlResourceTable = core::RenderResourceTable<GLuint, Resource>;

    std::vector<DeleteRequest> m_delete_queue; ///< @brief All objects queued for cleanup.

    GlResourceTable<core::Buffer> m_buffer_table;                      ///< @brief Buffer handle storage.
    GlResourceTable<core::Image> m_image_table;                        ///< @brief Image handle storage.
    GlResourceTable<core::Sampler> m_sampler_table;                    ///< @brief Sampler handle storage.
    GlResourceTable<core::Framebuffer> m_framebuffer_table;            ///< @brief Framebuffer handle storage.
    GlResourceTable<core::Shader> m_shader_table;                      ///< @brief Shader handle storage.
    GlResourceTable<core::GraphicsPipeline> m_graphics_pipeline_table; ///< @brief GraphicsPipeline handle storage.
};

} // namespace siren::platform
