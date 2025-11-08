#include "UniformBuffer.hpp"


namespace siren::core
{

UniformBuffer::UniformBuffer(const Vector<u8>& data, const BufferUsage usage)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_UNIFORM_BUFFER, data.size(), data.data(), static_cast<GLenum>(usage));
    unbind();
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void UniformBuffer::setData(const Vector<u8>& data, BufferUsage usage)
{
    bind();
    glBufferData(GL_UNIFORM_BUFFER, data.size(), data.data(), static_cast<GLenum>(usage));
    unbind();
}

void UniformBuffer::setData(const void* data, const u32 size, BufferUsage usage)
{
    bind();
    glBufferData(GL_UNIFORM_BUFFER, size, data, static_cast<GLenum>(usage));
    unbind();
}

void UniformBuffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void UniformBuffer::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::attach(const int location) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, location, m_id);
}

BufferID UniformBuffer::id() const
{
    return m_id;
}

} // namespace siren::core
