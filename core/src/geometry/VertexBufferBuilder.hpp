#pragma once
#include "../renderer/resources/Buffer.hpp"
#include "../renderer/VertexLayout.hpp"


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

class VertexBufferBuilder {
public:
    explicit VertexBufferBuilder(const VertexLayout& layout);

    void push_vertex(const CompleteVertex& vertex);
    Buffer build();
    u32 get_size() const;

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
