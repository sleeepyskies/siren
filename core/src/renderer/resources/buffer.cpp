#include "buffer.hpp"

#include "renderer/device.hpp"


namespace siren::core
{

Buffer::Buffer(
    Device* device,
    const BufferHandle handle,
    const BufferDescriptor& descriptor
) : Base(device, handle),
    m_descriptor(descriptor) { }

Buffer::~Buffer() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_buffer(m_handle);
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : Base(std::move(other)),
      m_descriptor(std::move(other.m_descriptor)) { }

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_buffer(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);
    }
    return *this;
}
auto Buffer::descriptor() const noexcept -> const BufferDescriptor& { return m_descriptor; }

auto Buffer::upload(std::span<const u8> data) const noexcept -> std::expected<void, Error> {
    if (!m_device) {
        return std::unexpected(Error{ Code::DeviceNotPresent });
    }

    auto recorder = m_device->record_commands();
    recorder->upload_to_buffer(m_handle, data);
    m_device->submit(std::move(recorder));

    return { };
}
} // namespace siren::core
