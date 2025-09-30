#include "Buffer.hpp"

namespace core::renderer
{

// ====================== VBO ======================
VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &m_id);
    trc("Generated VBO {}", m_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted VBO {}", m_id);
}

void VertexBuffer::bind() const
{
    assert(m_id != 0 && "Attempting to bind invalid VBO");
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::uploadData(const std::vector<Byte>& data, const BufferUsage usage)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), usage);
}

BufferID VertexBuffer::id() const
{
    return m_id;
}

// ====================== EBO ======================
IndexBuffer::IndexBuffer()
{
    glGenBuffers(1, &m_id);
    trc("Generated EBO {}", m_id);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted EBO {}", m_id);
}

void IndexBuffer::bind() const
{
    assert(m_id != 0 && "Attempting to bind invalid EBO");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::uploadIndices(const std::vector<uint32_t>& indices, const BufferUsage usage)
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), usage);
}

BufferID IndexBuffer::id() const
{
    return m_id;
}

// ====================== VAO ======================
VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
    trc("Generated VAO {}", m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted VAO {}", m_id);
}

void VertexArray::linkVertexBuffer(const VertexBuffer& VBO, const VertexBufferLayout& layout)
{
    // since gl functions operate on the currently bound buffer, we must bind
    bind();
    VBO.bind();

    // make this attribute available for shaders to read
    glEnableVertexAttribArray(layout.index);

    // describe this attributes layout in the VBO
    glVertexAttribPointer(layout.index,
                          layout.size,
                          layout.type,
                          layout.normalized,
                          layout.stride,
                          reinterpret_cast<void*>(layout.offset));

    VBO.unbind();
    unbind();
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

BufferID VertexArray::id() const
{
    return m_id;
}

// ====================== UBO ======================
UniformBuffer::UniformBuffer()
{
    glGenBuffers(1, &m_id);
    trc("Generated UBO {}", m_id);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted UBO {}", m_id);
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
    // TODO: give shader function to bind a uniform buffer
    glBindBufferRange(GL_UNIFORM_BUFFER, slot, m_id, 0, data.size());
}

BufferID UniformBuffer::id() const
{
    return m_id;
}

} // namespace core::renderer
