#pragma once

#include "renderer/Device.hpp"


namespace siren::platform
{

template <typename T, typename U>
class ResourceHandleTable {
public:
    using ApiHandleType   = T;
    using SirenHandleType = U;

    auto reserve() -> SirenHandleType {
        // free handles to use
        if (!m_free_list.empty()) {
            SirenHandleType back = m_free_list.back();
        }
    }

private:
    std::vector<ApiHandleType> m_table;
    std::vector<SirenHandleType> m_free_list;
};

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
    using GlResourceTable = ResourceHandleTable<GLuint, u32>;

    // std::vector<DeleteRequest> m_delete_queue;
    ResourceHandleTable<GLuint> m_buffer_table;
    ResourceHandleTable<GLuint> m_buffer_table;
};

} // namespace siren::platform
