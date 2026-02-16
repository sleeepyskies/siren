#pragma once
#include "renderer/resources/buffer.hpp"
#include "renderer/vertex_layout.hpp"


namespace siren::core
{
struct CompleteVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture;
    glm::vec4 color;
};

struct BufferParams {
    std::vector<u8> data;
};

class VertexBufferBuilder {
public:
    explicit VertexBufferBuilder(const VertexLayout& layout);

    auto push_vertex(const CompleteVertex& vertex) -> void;
    auto build() -> BufferParams;
    auto get_size() const -> u32;

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
} // namespace siren::core
