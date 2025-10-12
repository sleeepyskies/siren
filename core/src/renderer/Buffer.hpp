#pragma once

#include "BufferCommon.hpp"
#include "core/GL.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

/**
 * @brief Vertex Array Object. Represents a block of GPU memory describing how
 * to read from a Vertex Buffer. This means it describes the layout of the data.
 * Furthermore, a VAO simplifies logic, as once set up and bound to a VBO and
 * EBO, only the VAO must be bound again before each draw call is performed.
 */
class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void linkVertexBuffer(const Ref<VertexBuffer>& VBO);
    void linkIndexBuffer(const Ref<IndexBuffer>& EBO);
    Ref<VertexBuffer> getVertexBuffer() const;
    Ref<IndexBuffer> getIndexBuffer() const;
    void bind() const;
    void unbind() const;
    BufferID id() const;
    bool hasAttribute(AllowedShaderAttribute attribute) const;

private:
    BufferID m_id = 0;

    Ref<VertexBuffer> m_vertexBuffer = nullptr;
    Ref<IndexBuffer> m_indexBuffer   = nullptr;
};

class UniformBuffer
{
public:
    UniformBuffer();
    ~UniformBuffer();

    void bind() const;
    void unbind() const;
    void uploadData(const std::vector<Byte>& data, BufferUsage usage, uint8_t slot) const;
    BufferID id() const;

private:
    BufferID m_id = 0;
};

} // namespace siren::renderer
