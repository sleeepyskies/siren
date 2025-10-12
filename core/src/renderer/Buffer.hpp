#pragma once

#include "BufferCommon.hpp"
#include "Shader.hpp"
#include "core/GL.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

/**
 * @brief Element Buffer Object. Represents a block of GPU memory storing
 * indices into a Vertex Buffer. These indices indicate the drawing order of vertices and
 * are used to reduce data duplication.
 */
class IndexBuffer
{
public:
    explicit IndexBuffer(IndexDataType type);
    ~IndexBuffer();

    void uploadIndices(const std::vector<Byte>& indices, BufferUsage usage);
    void bind() const;
    void unbind() const;
    IndexDataType getIndexType() const;
    size_t getIndexCount() const;
    BufferID id() const;

private:
    BufferID m_id = 0;
    IndexDataType m_type;
    size_t m_indicesCount;
};

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
