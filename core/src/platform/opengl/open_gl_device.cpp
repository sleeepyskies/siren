#include "open_gl_device.hpp"

#include "platform/GL.hpp"
#include "sync/render_thread.hpp"


namespace siren::platform
{

OpenGlDevice::OpenGlDevice() : Device() { }

OpenGlDevice::~OpenGlDevice() { }

auto OpenGlDevice::create_buffer(const core::BufferDescriptor& desc) -> core::Buffer {
    SIREN_ASSERT(desc.size > 0, "Cannot allocate empty buffer.");
    core::RenderResourceID<core::Buffer> handle;
    core::Locator<core::RenderThread>::locate().spawn(
        [&handle, desc] {
            glCreateBuffers(1, &handle.value);
            if (desc.name.has_value()) {
                glObjectLabel(GL_BUFFER, handle.value, desc.name.value().size(), desc.name.value().data());
            }
        }
    );
    return core::Buffer{ this, handle, desc.size, desc.usage };
}

auto OpenGlDevice::destroy_buffer(const core::BufferHandle handle) -> void {
    m_delete_queue.push_back({ handle.value, OpenGlResourceType::Buffer });
}

} // namespace siren::platform
