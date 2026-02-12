#include "vertex_layout.hpp"

#include "renderer/shaders/ShaderUtils.hpp"


namespace siren::core
{
VertexLayout::VertexLayout(std::vector<VertexAttribute>&& attributes) {
    set_layout(std::forward<std::vector<VertexAttribute>>(attributes));
}

void VertexLayout::set_layout(std::vector<VertexAttribute>&& attributes) {
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

std::vector<VertexElement> VertexLayout::get_elements() const {
    return m_elements;
}

u32 VertexLayout::get_vertex_stride() const {
    return m_stride;
}

bool VertexLayout::has_attribute(const VertexAttribute attribute) const {
    return m_attributes.contains(attribute);
}

u32 VertexLayout::get_element_offset(const VertexAttribute attribute) const {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.offset;
        }
    }
    return 0;
}

u32 VertexLayout::get_element_size(const VertexAttribute attribute) const {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.size;
        }
    }
    return 0;
}
} // namespace siren::core
