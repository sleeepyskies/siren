#include "Buffer.hpp"

namespace core
{

// ====================== VBO ======================
VBO::VBO()
{
    glGenBuffers(1, &m_id);
    trc("Generated VBO {}", m_id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted VBO {}", m_id);
}

void VBO::bind() const
{
    assert(m_id != 0 && "Attempting to bind invalid VBO");
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::uploadData(const std::vector<Vertex>& data, const BufferUsage usage)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), data.data(), usage);
}

GLuint VBO::id() const
{
    return m_id;
}

// ====================== EBO ======================
EBO::EBO()
{
    glGenBuffers(1, &m_id);
    trc("Generated EBO {}", m_id);
}

EBO::~EBO()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted EBO {}", m_id);
}

void EBO::bind() const
{
    assert(m_id != 0 && "Attempting to bind invalid EBO");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void EBO::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::uploadIndices(const std::vector<uint32_t>& indices, const BufferUsage usage)
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), usage);
}

GLuint EBO::id() const
{
    return m_id;
}

// ====================== VAO ======================
VAO::VAO()
{
    glGenVertexArrays(1, &m_id);
    trc("Generated VAO {}", m_id);
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted VAO {}", m_id);
}

void VAO::linkAttribute(const VBO& VBO, const AttributeLayout& layout)
{
    // since gl functions operate on the currently bound buffer, we must bind
    VBO.bind();

    // describe this attributes layout in the VBO
    glVertexAttribPointer(layout.index,
                          layout.size,
                          layout.type,
                          layout.normalized,
                          layout.stride,
                          reinterpret_cast<void*>(layout.offset));
    // make this attribute available for shaders to read
    glEnableVertexAttribArray(layout.index);

    VBO.unbind();
}

void VAO::bind() const
{
    glBindVertexArray(m_id);
}

void VAO::unbind() const
{
    glBindVertexArray(0);
}

GLuint VAO::id() const
{
    return m_id;
}

// ====================== UBO ======================
UBO::UBO()
{
    glGenBuffers(1, &m_id);
    trc("Generated UBO {}", m_id);
}

UBO::~UBO()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted UBO {}", m_id);
}

void UBO::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void UBO::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::uploadData(const std::vector<unsigned char>& data, const BufferUsage usage,
                     const uint8_t slot) const
{
    bind();
    glBufferData(GL_UNIFORM_BUFFER, data.size(), data.data(), usage);
    unbind();
    // TODO: give shader function to bind a uniform buffer
    glBindBufferRange(GL_UNIFORM_BUFFER, slot, m_id, 0, data.size());
}

GLuint UBO::id() const
{
    return m_id;
}

} // namespace core
