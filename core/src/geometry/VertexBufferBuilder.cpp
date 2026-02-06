#include "VertexBufferBuilder.hpp"

#include "core/assert.hpp"


namespace siren::core
{
VertexBufferBuilder::VertexBufferBuilder(const VertexLayout& layout) : m_layout(layout) {
    static const std::vector<std::pair<VertexAttribute, u32>> map = {
        { VertexAttribute::Position, offsetof(CompleteVertex, position) },
        { VertexAttribute::Normal, offsetof(CompleteVertex, normal) },
        { VertexAttribute::Tangent, offsetof(CompleteVertex, tangent) },
        { VertexAttribute::Bitangent, offsetof(CompleteVertex, bitangent) },
        { VertexAttribute::Texture, offsetof(CompleteVertex, texture) },
        { VertexAttribute::Color, offsetof(CompleteVertex, color) }
    };

    SIREN_ASSERT(layout.has_attribute(VertexAttribute::Position), "Meshes must have a position attribute");

    for (const auto& [attr, srcOff] : map) {
        if (m_layout.has_attribute(attr)) {
            m_copy_definitions.push_back(
                {
                    .srcOffset = srcOff,
                    .destOffset = m_layout.get_element_offset(attr),
                    .size = m_layout.get_element_size(attr) * (u32)sizeof(float)
                }
            );
        }
    }
}

void VertexBufferBuilder::push_vertex(const CompleteVertex& vertex) {
    m_count++;

    const u32 previousSize = m_data.size();
    m_data.resize(previousSize + m_layout.get_vertex_stride());

    for (const auto& cd : m_copy_definitions) {
        std::memcpy(
            m_data.data() + previousSize + cd.destOffset,
            reinterpret_cast<const u8*>(&vertex) + cd.srcOffset,
            cd.size
        );
    }
}

Buffer VertexBufferBuilder::build() {
    return Buffer{ std::span(m_data.data(), m_data.size()), BufferUsage::Static };
}

u32 VertexBufferBuilder::get_size() const {
    return m_count;
}
} // namespace siren::core
