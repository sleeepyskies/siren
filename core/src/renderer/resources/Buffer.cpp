#include "Buffer.hpp"

#include "renderer/Device.hpp"


namespace siren::core
{
Buffer::Buffer(
    Device* device,
    const BufferHandle handle,
    const usize size,
    const BufferUsage usage
) : RenderResource(device, handle), m_size(size), m_usage(usage) { }

Buffer::~Buffer() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_buffer(m_handle);
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : RenderResource(std::move(other)), m_size(std::exchange(other.m_size, 0)), m_usage(other.m_usage) { }

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_buffer(m_handle);
        }

        // parent move
        RenderResource<Buffer>::operator=(std::move(other));

        m_size  = std::exchange(other.m_size, 0);
        m_usage = other.m_usage;
    }
    return *this;
}

auto Buffer::size() const noexcept -> usize { return m_size; }
auto Buffer::usage() const noexcept -> BufferUsage { return m_usage; }

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
