#include "VertexBuffer.hpp"

#include "core/GL.hpp"

namespace siren::renderer
{

VertexBuffer::VertexBuffer(const std::vector<Byte>& data, const BufferUsage usage,
                           const VertexBufferLayout& layout)
    : m_layout(layout)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), static_cast<GLenum>(usage));
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() const
{
    SirenAssert(m_id != 0, "Attempting to bind an invalid VertexBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBufferLayout VertexBuffer::getLayout() const
{
    return m_layout;
}

} // namespace siren::renderer
