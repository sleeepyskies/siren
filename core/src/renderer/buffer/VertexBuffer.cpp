#include "VertexBuffer.hpp"

#include "../../platform/GL.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

static VertexBufferLayout computeLayout(const VertexData& vertexData)
{
    VertexBufferLayout layout{ };

    // todo: put back if checks, but requires flexible shaders first

    SirenAssert(!vertexData.positions.empty(), "VertexData must contain positions.");
    layout.addVertexAttribute(VertexAttribute::POSITION);

    // if (!vertexData.normals.empty()) {
    layout.addVertexAttribute(VertexAttribute::NORMAL);
    // }

    // if (!vertexData.tangents.empty()) {
    layout.addVertexAttribute(VertexAttribute::TANGENT);
    // }

    // if (!vertexData.bitangents.empty()) {
    layout.addVertexAttribute(VertexAttribute::BITANGENT);
    // }

    // if (!vertexData.textureUvs.empty()) {
    layout.addVertexAttribute(VertexAttribute::TEXTUREUV);
    // }

    // if (!vertexData.colors.empty()) {
    layout.addVertexAttribute(VertexAttribute::COLOR);
    // }

    return layout;
}

VertexBuffer::VertexBuffer(const VertexData& data, const BufferUsage usage)
    : m_layout(computeLayout(data))
{
    // Pre-allocate bytes to required size to avoid multiple reallocations
    const u32 vertexCount = data.positions.size();
    std::vector<u8> bytes(vertexCount * m_layout.getStride());

    for (u32 i = 0; i < vertexCount; i++) {
        u32 offset = 0;
        auto push  = [&] (auto& src, const auto& def) -> void {
            if (!src.empty()) {
                std::memcpy(
                    bytes.data() + i * m_layout.getStride() + offset,
                    &src[i],
                    sizeof(src[i])
                );
            } else {
                std::memcpy(bytes.data() + i * m_layout.getStride() + offset, &def, sizeof(def));
            }
            offset += sizeof(def);
        };

        push(data.positions, glm::vec3(0.0f));
        push(data.normals, glm::vec3(0.0f, 1.0f, 0.0f));
        push(data.tangents, glm::vec3(1.0f, 0.0f, 0.0f));
        push(data.bitangents, glm::vec3(0.0f, 0.0f, 1.0f));
        push(data.textureUvs, glm::vec2(0.0f));
        push(data.colors, glm::vec4(1.0f));
    }

    glGenBuffers(1, &m_id);
    bind();
    glBufferData(
        GL_ARRAY_BUFFER,
        bytes.size(),
        bytes.data(),
        static_cast<GLenum>(usage)
    );
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() const
{
    SirenAssert(m_id != 0, "Attempting to bind an invalid VertexBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBufferLayout VertexBuffer::getLayout() const
{
    return m_layout;
}

} // namespace siren::core
