#pragma once

#include "BufferCommon.hpp"

namespace siren::renderer
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
    explicit IndexBuffer(const std::vector<uint32_t>& indices);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    size_t getIndexCount() const;
    GLenum getIndexType() const;

private:
    BufferID m_id = 0;
    size_t m_indicesCount;
};

} // namespace siren::renderer
