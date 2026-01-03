#include "VertexLayout.hpp"

#include "renderer/shaders/ShaderUtils.hpp"


namespace siren::core
{
VertexLayout::VertexLayout(const std::initializer_list<VertexAttribute> attributes)
{
    SetLayout(attributes);
}

void VertexLayout::SetLayout(const std::initializer_list<VertexAttribute> attributes)
{
    m_stride = 0;

    for (const auto a : attributes) {
        m_attributes.insert(a);
        const i32 size = toComponentCount(a);

        m_elements.push_back(
            VertexElement{
                .attribute = a,
                .size = size,
                .type = toGLType(a),
                .normalized = false,
                .offset = m_stride,
            }
        );

        m_stride += size * sizeof(float); // for now, everything is a float
    }
}

Vector<VertexElement> VertexLayout::GetElements() const
{
    return m_elements;
}

u32 VertexLayout::GetVertexStride() const
{
    return m_stride;
}

bool VertexLayout::HasAttribute(const VertexAttribute attribute) const
{
    return m_attributes.contains(attribute);
}

u32 VertexLayout::GetElementOffset(const VertexAttribute attribute) const
{
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.offset;
        }
    }
    return 0;
}

u32 VertexLayout::GetElementSize(const VertexAttribute attribute) const
{
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.size;
        }
    }
    return 0;
}
} // namespace siren::core
