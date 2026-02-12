/**
 * @file Buffer.hpp
 */
#pragma once

#include "../RenderResource.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

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

struct Buffer;
using BufferHandle = RenderResourceID<Buffer>;

/**
 * @brief Describes a @ref Buffer. Used for object creation via @ref Device.
 */
struct BufferDescriptor {
    /// @brief An optional name. Mainly useful for debugging.
    std::optional<std::string> name;
    /// @brief The initial size of the buffer.
    usize size;
    /// @brief The intended use of the buffer.
    BufferUsage usage;
};

/**
 * @brief The Buffer class represents a chunk of GPU memory.
 * This makes use of OpenGL DSA and can thus be used as a buffer
 * for arbitrary types of data (vertex buffers, index buffers etc...)
 */
struct Buffer final : RenderResource<Buffer> {
    Buffer(Device* device, BufferHandle handle, usize size, BufferUsage usage);
    ~Buffer();

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    /// @brief Returns the allocated size of this buffer.
    [[nodiscard]] auto size() const noexcept -> usize;
    /// @brief Returns the @ref BufferUsage.
    [[nodiscard]] auto usage() const noexcept -> BufferUsage;
    /// @brief Upload data to this buffer.
    auto upload(std::span<const u8> data) const noexcept -> std::expected<void, Error>;

private:
    usize m_size;        ///< @brief Size of the buffer in bytes.
    BufferUsage m_usage; ///< @brief The buffer usage.
};
} // namespace siren::core
