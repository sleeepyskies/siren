module;

#include <vector>
#include <libassert/assert.hpp>

export module siren.render:vertex_buffer_builder;

import :vertex_layout;

import siren.common;
import siren.math;

namespace siren::render {

export struct CompleteVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture;
    glm::vec4 color;
};

export struct BufferParams {
    std::vector<u8> data;
};

export class VertexBufferBuilder {
public:
    explicit VertexBufferBuilder(const VertexLayout& layout);

    auto push_vertex(const CompleteVertex& vertex) -> void;
    auto build() -> BufferParams;
    auto size() const -> u32;

private:
    struct CopyDefinition {
        u32 srcOffset;
        u32 destOffset;
        u32 size;
    };

    std::vector<CopyDefinition> m_copy_definitions;
    std::vector<u8> m_data{ };
    VertexLayout m_layout;
    u32 m_count = 0;
};

VertexBufferBuilder::VertexBufferBuilder(const VertexLayout& layout) : m_layout(layout) {
    static const std::vector<std::pair<VertexAttribute, u32>> map = {
        { VertexAttribute::Position, offsetof(CompleteVertex, position) },
        { VertexAttribute::Normal, offsetof(CompleteVertex, normal) },
        { VertexAttribute::Tangent, offsetof(CompleteVertex, tangent) },
        { VertexAttribute::Bitangent, offsetof(CompleteVertex, bitangent) },
        { VertexAttribute::Texture, offsetof(CompleteVertex, texture) },
        { VertexAttribute::Color, offsetof(CompleteVertex, color) }
    };

    ASSERT(layout.has_attribute(VertexAttribute::Position), "Meshes must have a position attribute");

    for (const auto& [attr, srcOff] : map) {
        if (m_layout.has_attribute(attr)) {
            m_copy_definitions.push_back(
                {
                    .srcOffset = srcOff,
                    .destOffset = m_layout.element_offset(attr),
                    .size = m_layout.element_size(attr) * (u32)sizeof(float)
                }
            );
        }
    }
}

void VertexBufferBuilder::push_vertex(const CompleteVertex& vertex) {
    m_count++;

    const u32 previousSize = m_data.size();
    m_data.resize(previousSize + m_layout.vertex_stride());

    for (const auto& cd : m_copy_definitions) {
        std::memcpy(
            m_data.data() + previousSize + cd.destOffset,
            reinterpret_cast<const u8*>(&vertex) + cd.srcOffset,
            cd.size
        );
    }
}

auto VertexBufferBuilder::build() -> BufferParams {
    return BufferParams{ .data = std::move(m_data) };
}

u32 VertexBufferBuilder::size() const {
    return m_count;
}

} // namespace siren::render
