#include "VertexBufferLayout.hpp"

#include "renderer/shaders/ShaderUtils.hpp"


namespace siren::core
{

void VertexBufferLayout::addVertexAttribute(const VertexAttribute& attribute)
{
    m_elements.emplace_back(attribute);

    u32 index        = 0;
    size_t stride    = 0;
    size_t offsetAcc = 0;

    for (const auto& elem : m_elements) {
        stride += sizeof(float) * toComponentCount(elem.attribute);
    }

    for (auto& elem : m_elements) {
        elem.index  = index++;
        elem.size   = toComponentCount(elem.attribute);
        elem.type   = toGLType(elem.attribute);
        elem.stride = stride;
        elem.offset = offsetAcc;
        offsetAcc += elem.size * sizeof(float);
    }
}

Vector<VertexBufferElement> VertexBufferLayout::getLayout() const
{
    return m_elements;
}

u32 VertexBufferLayout::getStride() const
{
    // fixme: this could be safer. maybe cache the stride?
    return m_elements.size() == 0 ? 0 : m_elements[0].stride;
}

} // namespace siren::core
