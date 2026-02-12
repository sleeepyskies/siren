#include "open_gl_device.hpp"

#include "platform/GL.hpp"
#include "sync/render_thread.hpp"


namespace siren::platform
{

OpenGlDevice::OpenGlDevice() : Device() { }

OpenGlDevice::~OpenGlDevice() { }

auto OpenGlDevice::create_buffer(const core::BufferDescriptor& desc) -> core::Buffer {
    SIREN_ASSERT(desc.size > 0, "Cannot allocate empty buffer.");
    auto buffer_handle = m_buffer_table.reserve();
    core::Locator<core::RenderThread>::locate().spawn(
        [buffer_handle, desc, this] {
            GLuint buf;
            glCreateBuffers(1, &buf);
            if (desc.name.has_value()) {
                glObjectLabel(GL_BUFFER, buf, desc.name.value().size(), desc.name.value().data());
            }

            this->m_buffer_table.link(buffer_handle, buf);
        }
    );
    return core::Buffer{ this, buffer_handle, desc.size, desc.usage };
}

auto OpenGlDevice::destroy_buffer(const core::BufferHandle handle) -> void {
    // opengl ignores 0 values, so we don't need to check
    const auto api_handle = m_buffer_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Buffer });
}

auto OpenGlDevice::create_image(const core::ImageDescriptor& desc) -> core::Image { }

auto OpenGlDevice::destroy_image(core::ImageHandle handle) -> void { }

} // namespace siren::platform
