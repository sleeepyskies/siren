#pragma once

#include "BufferCommon.hpp"
#include "VertexBufferLayout.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

// todo: maybe an copyToBuffer() and uploadData() api is cleaner?

/**
 * @brief Vertex Buffer Object. Represents a raw block of GPU memory storing vertex
 * data. This may be positions, colors, normals, texture coordinates etc...
 */
class VertexBuffer
{
public:
    /// @brief Creates a CPU side object as well as uploads the data to the GPU
    VertexBuffer(const std::vector<Byte>& data, BufferUsage usage,
                 const VertexBufferLayout& layout);
    ~VertexBuffer();

    /// @brief Binds the VertexBuffer, aka sets it as the currently active Vertex Buffer for OpenGL
    void bind() const;
    /// @brief Unbinds whatever VertexBuffer is currently bound.
    void unbind() const;

    /// @brief Returns the current layout
    VertexBufferLayout getLayout() const;

private:
    BufferID m_id = 0;
    VertexBufferLayout m_layout{};
};

} // namespace siren::renderer
