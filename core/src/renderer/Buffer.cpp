#include "Buffer.hpp"

namespace core::renderer
{
// ==================== Helpers =====================
inline size_t shaderTypeSize(const GLenum type)
{
    switch (type) {
    case GL_BYTE          : return 1;
    case GL_UNSIGNED_BYTE : return 1;
    case GL_SHORT         : return 2;
    case GL_UNSIGNED_SHORT: return 2;
    case GL_UNSIGNED_INT  : return 4;
    case GL_FLOAT         : return 4;
    default               : SirenAssert(false, "Invalid shader type");
    }
}

// ==================== Layout =====================
void VertexBufferLayout::addVertexAttribute(const VertexBufferAttribute& attribute)
{
    SirenAssert(!m_closed, "Cannot add more attribtues to a closed VertexBufferLayout");
    m_attributes.push_back(attribute);
}

void VertexBufferLayout::close()
{
    SirenAssert(!m_closed, "Cannot close an already closed VertexBufferLayout");
    m_closed = true;

    size_t offsetAcc = 0;
    size_t stride    = 0;

    for (auto& a : m_attributes) {
        stride += a.size * shaderTypeSize(a.type);
    }

    size_t index = 0;
    for (auto& a : m_attributes) {
        a.offset = offsetAcc;
        a.stride = stride;
        a.index  = index++;

        offsetAcc += a.size * shaderTypeSize(a.type);
    }
}

// ====================== VBO ======================
VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &m_id);
    trc("Generated Vertex Buffer {}", m_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted Vertex Buffer {}", m_id);
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
IndexBuffer::IndexBuffer(const IndexDataType type) : m_type(type)
{
    glGenBuffers(1, &m_id);
    trc("Generated Index Buffer {}", m_id);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
    trc("Deleted Index Buffer {}", m_id);
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

void IndexBuffer::uploadIndices(const std::vector<Byte>& indices, const BufferUsage usage)
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), indices.data(), usage);
}

BufferID IndexBuffer::id() const
{
    return m_id;
}

// ====================== VAO ======================
VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
    trc("Generated Vertex Array {}", m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
    trc("Deleted Vertex Array {}", m_id);
}

void VertexArray::linkVertexBuffer(const ref<VertexBuffer>& VBO, const VertexBufferLayout& layout)
{
    // since gl functions operate on the currently bound buffer, we must bind
    bind();
    VBO->bind();

    for (const auto& attribute : layout.getLayout()) {
        // make this attribute visible for shaders
        glEnableVertexAttribArray(attribute.index);

        // describe the structure of this specific layout in this Vertex Buffer
        glVertexAttribPointer(attribute.index,
                              attribute.size,
                              attribute.type,
                              attribute.normalized,
                              attribute.stride,
                              reinterpret_cast<const void*>(attribute.offset));
    }

    VBO->unbind();
    unbind();

    m_vertexBuffer = VBO;
}

void VertexArray::linkIndexBuffer(const ref<IndexBuffer>& EBO)
{
    bind();
    EBO->bind();

    m_indexBuffer = EBO;

    EBO->unbind();
    unbind();
}

ref<VertexBuffer> VertexArray::getVertexBuffer() const
{
    return m_vertexBuffer;
}

ref<IndexBuffer> VertexArray::getIndexBuffer() const
{
    return m_indexBuffer;
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
    // TODO: give shader function to bind a uniform buffer
    glBindBufferRange(GL_UNIFORM_BUFFER, slot, m_id, 0, data.size());
}

BufferID UniformBuffer::id() const
{
    return m_id;
}

} // namespace core::renderer
