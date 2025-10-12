#include "UniformBuffer.hpp"

namespace siren::renderer
{

UniformBuffer::UniformBuffer(const std::vector<Byte>& data, const BufferUsage usage)
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

} // namespace siren::renderer