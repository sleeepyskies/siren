#pragma once

#include "renderer/Device.hpp"


namespace siren::platform
{

enum class OpenGlResourceType {
    Buffer,
    Image,
    Sampler,
    Framebuffer,
    Shader,
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

private:
    std::vector<DeleteRequest> m_delete_queue;
};

} // namespace siren::platform
