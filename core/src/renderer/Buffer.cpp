#include "Buffer.hpp"

namespace siren::renderer
{
// ====================== UBO ======================
UniformBuffer::UniformBuffer()
{
    glGenBuffers(1, &m_id);
    trc("Generated Uniform Buffer {}", m_id);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted Uniform Buffer {}", m_id);
}

void UniformBuffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void UniformBuffer::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::uploadData(const std::vector<Byte>& data, const BufferUsage usage,
                               const uint8_t slot) const
{
    bind();
    glBufferData(GL_UNIFORM_BUFFER, data.size(), data.data(), usage);
    unbind();
}

BufferID UniformBuffer::id() const
{
    return m_id;
}

} // namespace siren::renderer
