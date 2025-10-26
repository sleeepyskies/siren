/**
 * @file IndexBuffer.hpp
 */
#pragma once

#include "BufferCommon.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

/**
 * @brief Represents a block of GPU memory storing indices into a Vertex Buffer. These indices
 * indicate the drawing order of vertices and are used to reduce data duplication.
 *
 * @note siren only supports 32-bit indices
 * @note IndexBuffers currently only support buffer usage Static Draw
 */
class IndexBuffer
{
public:
    /// @brief Creates a new IndexBuffer and uploads the indices to the GPU
    explicit IndexBuffer(const std::vector<u32>& indices);
    ~IndexBuffer();

    /// @brief Binds the IndexBuffer, aka sets it as the currently active IndexBuffer for OpenGL
    void bind() const;
    /// @brief Unbinds whatever IndexBuffer is currently bound.
    void unbind() const;
    /// @brief Returns the amount of indices in this buffer
    size_t getIndexCount() const;
    /// @brief Returns the type of the indices in this buffer
    GLenum getIndexType() const;

private:
    BufferID m_id         = 0;
    size_t m_indicesCount = 0;
};

} // namespace siren::core
