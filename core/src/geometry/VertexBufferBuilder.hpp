#pragma once
#include "renderer/buffer/Buffer.hpp"
#include "renderer/buffer/VertexLayout.hpp"


namespace siren::core
{
struct CompleteVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture;
    glm::vec4 color;
};

class VertexBufferBuilder
{
public:
    explicit VertexBufferBuilder(const VertexLayout& layout);

    void PushVertex(const CompleteVertex& vertex);
    Ref<Buffer> Build() const;
    u32 GetSize() const;

private:
    struct CopyDefinition
    {
        u32 srcOffset;
        u32 destOffset;
        u32 size;
    };

    Vector<CopyDefinition> m_copyDefinitions;
    Vector<u8> m_data{ };
    VertexLayout m_layout;
    u32 m_count = 0;
};
} // namespace siren::core
