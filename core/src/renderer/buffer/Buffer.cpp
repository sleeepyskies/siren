#include "Buffer.hpp"

#include "platform/GL.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
Buffer::Buffer(const void* data, const size_t size, const BufferUsage usage) : m_id(0), m_size(size), m_usage(usage)
{
    SirenAssert(m_size > 0, "Cannot upload an empty buffer");
    const auto bytes = static_cast<const u8*>(data);
    glCreateBuffers(1, &m_id);
    glNamedBufferData(m_id, size, bytes, static_cast<GLenum>(m_usage));
}

Buffer::Buffer(const size_t size, const BufferUsage usage) : m_id(0), m_size(size), m_usage(usage)
{
    SirenAssert(m_size > 0, "Cannot upload an empty buffer");
    glCreateBuffers(1, &m_id);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_id);
}

u32 Buffer::GetID() const
{
    return m_id;
}

size_t Buffer::GetSize() const
{
    return m_size;
}

void Buffer::Update(const void* data, const size_t size)
{
    glNamedBufferSubData(m_id, 0, size, data);
    m_size = size;
}
} // namespace siren::core
