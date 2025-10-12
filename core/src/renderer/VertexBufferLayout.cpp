#include "VertexBufferLayout.hpp"

#include "ShaderUtils.hpp"

namespace siren::renderer
{

void VertexBufferLayout::addVertexAttribute(const ShaderAttribute& attribute)
{
    m_elements.emplace_back(attribute);

    uint32_t index     = 0;
    uint32_t stride    = 0;
    uint32_t offsetAcc = 0;

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

std::vector<VertexBufferElement> VertexBufferLayout::getLayout() const
{
    return m_elements;
}

} // namespace siren::renderer
