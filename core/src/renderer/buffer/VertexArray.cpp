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

void VertexArray::linkVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
    // since gl functions operate on the currently bound buffer, we must bind
    bind();
    vertexBuffer->bind();

    for (const auto& attribute : vertexBuffer->getLayout().getLayout()) {
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

    vertexBuffer->unbind();
    unbind();

    m_vertexBuffer = vertexBuffer;
}

void VertexArray::linkIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
    bind();
    indexBuffer->bind();

    m_indexBuffer = indexBuffer;

    unbind();
    // we have to unbind vertex array BEFORE unbinding index buffer!!
    indexBuffer->unbind();
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

Ref<VertexBuffer> VertexArray::getVertexBuffer() const
{
    return m_vertexBuffer;
}

Ref<IndexBuffer> VertexArray::getIndexBuffer() const
{
    return m_indexBuffer;
}

} // namespace siren::renderer
