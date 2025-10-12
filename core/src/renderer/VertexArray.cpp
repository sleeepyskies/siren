#include "VertexArray.hpp"

namespace siren::renderer
{

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
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
    // WE HAVE TO UNBIND VERTEX ARRAY BEFORE UNBIND INDEX BUFFER!!
    EBO->unbind();
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

} // namespace siren::renderer
