#include "Buffer.hpp"

#include "core/assert.hpp"
#include "platform/GL.hpp"


namespace siren::core
{
Buffer::Buffer(
    const std::span<u8> data,
    const BufferUsage usage
) : m_handle(0), m_size(data.size_bytes()), m_usage(usage) {
    SIREN_ASSERT(m_size > 0, "Cannot upload an empty buffer");
    glCreateBuffers(1, &m_handle.value);
    glNamedBufferData(m_handle.value, m_size, data.data(), static_cast<GLenum>(m_usage));
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_handle.value);
}

Buffer::Buffer(Buffer&& other) noexcept : m_handle(other.handle()), m_size(other.size()), m_usage(other.usage()) {
    other.m_size   = 0;
    other.m_handle = BufferHandle::invalid();
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        m_handle = other.handle();
        m_usage  = other.usage();
        m_size   = other.size();

        other.m_size   = 0;
        other.m_handle = BufferHandle::invalid();
    }
    return *this;
}

auto Buffer::handle() const -> BufferHandle { return m_handle; }

auto Buffer::size() const -> usize { return m_size; }

auto Buffer::usage() const -> BufferUsage { return m_usage; }

auto Buffer::update(const std::span<u8> data) -> void {
    glNamedBufferSubData(m_handle.value, 0, data.size_bytes(), data.data());
    m_size = data.size_bytes();
}
} // namespace siren::core
