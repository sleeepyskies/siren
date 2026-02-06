/**
 * @file Buffer.hpp
 */
#pragma once

#include "GpuResource.hpp"
#include "core/Core.hpp"


namespace siren::core
{

class Buffer;
using BufferHandle = GpuResourceHandle<Buffer>;

/**
 * @brief Defines the usage of a Buffer.
 */
enum class BufferUsage {
    /// @brief Best used for data that is set once and persistent such as static
    /// meshes.
    Static,
    /// @brief Best used for data is updated often and persistent like dynamic
    /// meshes.
    Dynamic,
    /// @brief Best used for data that is updated often but not persistent like
    /// UI or short term effects.
    Stream,
};

/**
 * @brief The Buffer class represents a chunk of GPU memory.
 * This makes use of openGL DSA and can thus be used as a buffer
 * for arbitrary types of data (vertex buffers, index buffers etc...)
 */
class Buffer final : public GpuResource {
public:
    /// @brief Creates a CPU side object as well as uploads the data to the GPU
    Buffer(std::span<u8> data, BufferUsage usage);
    ~Buffer() override;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    /// @brief Returns this Buffer's ID.
    auto handle() const -> BufferHandle;
    /// @brief Returns the size of this buffer.
    auto size() const -> usize;
    /// @brief Returns the BufferUsage of this buffer.
    auto usage() const -> BufferUsage;
    /// @brief Updates this buffers data
    auto update(std::span<u8> data) -> void;

private:
    BufferHandle m_handle;
    usize m_size;
    BufferUsage m_usage;
};
} // namespace siren::core
