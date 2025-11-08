#include "IndexBuffer.hpp"


namespace siren::core
{

IndexBuffer::IndexBuffer(const Vector<u32>& indices) : m_indicesCount(indices.size())
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(u32),
        indices.data(),
        GL_STATIC_DRAW
    );
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::bind() const
{
    SirenAssert(m_id != 0, "Attempting to bind an invalid IndexBuffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t IndexBuffer::getIndexCount() const
{
    return m_indicesCount;
}

GLenum IndexBuffer::getIndexType() const
{
    return GL_UNSIGNED_INT;
}

} // namespace siren::core
