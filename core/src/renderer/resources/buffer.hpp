#pragma once

#include "renderer/render_resource.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

class Buffer;
using BufferHandle = RenderResourceID<Buffer>;

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
 * @brief Describes a @ref Buffer. Used for object creation via @ref Device.
 */
struct BufferDescriptor {
    /// @brief An optional label. Mainly useful for debugging.
    std::optional<std::string> label;
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
class Buffer final : public RenderResource<Buffer> {
    using Base = RenderResource<Buffer>;

public:
    Buffer(
        Device* device,
        BufferHandle handle,
        const BufferDescriptor& descriptor
    );
    ~Buffer();

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    /// @brief Returns the optional label of this buffer.
    [[nodiscard]] auto label() const noexcept -> std::optional<std::string_view>;
    /// @brief Returns the allocated size of this buffer.
    [[nodiscard]] auto size() const noexcept -> usize;
    /// @brief Returns the @ref BufferUsage.
    [[nodiscard]] auto usage() const noexcept -> BufferUsage;

    /// @brief Upload data to this Buffer.
    [[nodiscard]] auto upload(std::span<const u8> data) const noexcept -> std::expected<void, Error>;

private:
    BufferDescriptor m_descriptor; ///< @brief Data of the buffer.
};
} // namespace siren::core
