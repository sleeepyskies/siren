/**
 * @file Buffer.hpp
 */
#pragma once

#include "utilities/spch.hpp"
#include "platform/GL.hpp"


namespace siren::core
{
/**
 * @brief Defines the usage of a Buffer.
 */
enum class BufferUsage
{
    /// @brief Best used for data that is set once and persistent such as static
    /// meshes.
    Static = GL_STATIC_DRAW,
    /// @brief Best used for data is updated often and persistent like dynamic
    /// meshes.
    Dynamic = GL_DYNAMIC_DRAW,
    /// @brief Best used for data that is updated often but not persistent like
    /// UI or short term effects.
    Stream = GL_STREAM_DRAW,
};

/**
 * @brief The Buffer class represents a chunk of GPU memory.
 * This makes use of openGL DSA and can thus be used as a buffer
 * for arbitrary types of data (vertex buffers, index buffers etc...)
 */
class Buffer
{
public:
    /// @brief Creates a CPU side object as well as uploads the data to the GPU
    Buffer(const void* data, size_t size, BufferUsage usage);
    ~Buffer();

    Buffer(Buffer&)            = delete;
    Buffer& operator=(Buffer&) = delete;

    /// @brief Returns this Buffer's ID.
    u32 GetID() const;
    /// @brief Returns the size of this buffer.
    size_t GetSize() const;
    /// @brief Updates this buffers data
    void Update(const void* data, size_t size);

private:
    u32 m_id;
    size_t m_size;
    BufferUsage m_usage;
};
} // namespace siren::core
