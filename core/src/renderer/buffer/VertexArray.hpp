#pragma once

#include "BufferCommon.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

namespace siren::core
{

/**
 * @brief Represents a block of GPU memory describing how
 * to read from a Vertex Buffer. This means it describes the layout of the data.
 * Furthermore, a VAO simplifies logic, as once set up and bound to a VBO and
 * EBO, only the VAO must be bound again before each draw call is performed.
 *
 * @note siren currently only supports one VertexBuffer per VertexArray
 * @note siren does not support non indexed drawing, so an IndexBuffer must always be provided
 */
class VertexArray
{
public:
    /// @brief Creates a new VertexArray. Must upload data first for it to be usable
    VertexArray();
    ~VertexArray();

    /// @brief Links the given VertexBuffer to this VertexArray and send its layout for the GPU
    void linkVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
    /// @brief Links the given IndexBuffer to this VertexArray
    void linkIndexBuffer(const Ref<IndexBuffer>& indexBuffer);
    /// @brief Binds the VertexArray, as well as implicitly bindings its VertexBOffer and
    /// IndexBuffer
    void bind() const;
    /// @brief Unbinds whatever VertexArray is currently bound.
    void unbind() const;

    /// @brief Returns the currently linked VertexBuffer
    Ref<VertexBuffer> getVertexBuffer() const;
    /// @brief Returns the currently linked IndexBuffer
    Ref<IndexBuffer> getIndexBuffer() const;

private:
    BufferID m_id                    = 0;
    Ref<VertexBuffer> m_vertexBuffer = nullptr;
    Ref<IndexBuffer> m_indexBuffer   = nullptr;
};

}; // namespace siren::core
