#include "VertexBufferBuilder.hpp"


namespace siren::core
{
VertexBufferBuilder::VertexBufferBuilder(const VertexLayout& layout) : m_layout(layout)
{
    static const std::vector<std::pair<VertexAttribute, u32>> map = {
        { VertexAttribute::Position, offsetof(CompleteVertex, position) },
        { VertexAttribute::Normal, offsetof(CompleteVertex, normal) },
        { VertexAttribute::Tangent, offsetof(CompleteVertex, tangent) },
        { VertexAttribute::Bitangent, offsetof(CompleteVertex, bitangent) },
        { VertexAttribute::Texture, offsetof(CompleteVertex, texture) },
        { VertexAttribute::Color, offsetof(CompleteVertex, color) }
    };

    SirenAssert(layout.HasAttribute(VertexAttribute::Position), "Meshes must have a position attribute");

    for (const auto& [attr, srcOff] : map) {
        if (m_layout.HasAttribute(attr)) {
            m_copyDefinitions.push_back(
                {
                    .srcOffset = srcOff,
                    .destOffset = m_layout.GetElementOffset(attr),
                    .size = m_layout.GetElementSize(attr) * (u32)sizeof(float)
                }
            );
        }
    }
}

void VertexBufferBuilder::PushVertex(const CompleteVertex& vertex)
{
    m_count++;

    const u32 previousSize = m_data.size();
    m_data.resize(previousSize + m_layout.GetVertexStride());

    for (const auto& cd : m_copyDefinitions) {
        std::memcpy(
            m_data.data() + previousSize + cd.destOffset,
            reinterpret_cast<const u8*>(&vertex) + cd.srcOffset,
            cd.size
        );
    }
}

Ref<Buffer> VertexBufferBuilder::Build() const
{
    return CreateRef<Buffer>(m_data.data(), m_data.size(), BufferUsage::Static);
}

u32 VertexBufferBuilder::GetSize() const
{
    return m_count;
}
} // namespace siren::core
