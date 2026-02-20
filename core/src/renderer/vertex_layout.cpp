#include "vertex_layout.hpp"

#include "renderer/shaders/ShaderUtils.hpp"


namespace siren::core
{
VertexLayout::VertexLayout(std::vector<VertexAttribute>&& attributes) {
    layout(std::forward<std::vector<VertexAttribute>>(attributes));
}

auto VertexLayout::layout(std::vector<VertexAttribute>&& attributes) -> void {
    m_stride = 0;

    for (const auto a : attributes) {
        m_attributes.insert(a);
        const u32 size = toComponentCount(a);

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

auto VertexLayout::elements() const -> std::vector<VertexElement> {
    return m_elements;
}

auto VertexLayout::vertex_stride() const -> u32 {
    return m_stride;
}

auto VertexLayout::has_attribute(const VertexAttribute attribute) const -> bool {
    return m_attributes.contains(attribute);
}

auto VertexLayout::element_offset(const VertexAttribute attribute) const -> u32 {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.offset;
        }
    }
    return 0;
}

auto VertexLayout::element_size(const VertexAttribute attribute) const -> u32 {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.size;
        }
    }
    return 0;
}
} // namespace siren::core
