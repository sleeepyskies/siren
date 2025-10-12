#include "Buffer.hpp"

namespace siren::renderer
{

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

void VertexArray::linkVertexBuffer(const Ref<VertexBuffer>& VBO)
{
    // since gl functions operate on the currently bound buffer, we must bind
    bind();
    VBO->bind();
    const VertexBufferLayout& layout = VBO->getLayout();

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

void VertexArray::linkIndexBuffer(const Ref<IndexBuffer>& EBO)
{
    bind();
    EBO->bind();

    m_indexBuffer = EBO;

    unbind();
    // WE HAVE OT UNBID VERTEX ARRAY BEFORE UNBIND INDEX BUFFER
    EBO->unbind();
}

Ref<VertexBuffer> VertexArray::getVertexBuffer() const
{
    return m_vertexBuffer;
}

Ref<IndexBuffer> VertexArray::getIndexBuffer() const
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

bool VertexArray::hasAttribute(const AllowedShaderAttribute attribute) const
{
    return m_vertexBuffer->getLayout().hasAttribute(attribute);
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
}

BufferID UniformBuffer::id() const
{
    return m_id;
}

} // namespace siren::renderer
