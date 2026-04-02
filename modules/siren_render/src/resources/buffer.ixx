module;

#include "assert.hpp"
#include <optional>

export module siren.render.buffer;

import siren.render.render_resource;
import siren.render.device;
import siren.common;

namespace siren::render {

/** @brief Defines the usage of a Buffer. */
export enum class BufferUsage {
    /** @brief Best used for data that is set once and persistent such as static meshes. */
    Static,
    /** @brief Best used for data is updated often and persistent like dynamic meshes. */
    Dynamic,
    /** @brief Best used for data that is updated often but not persistent like UI or short term effects. */
    Stream,
};

/**
 * @brief Defines the index format of an index buffer.
 * @note We do this weird thing to get kinda enum member functions.
 */
export class IndexFormat {
public:
    enum Enum : u8 {
        /** @brief 8 bit long indices. */
        Byte8,
        /** @brief 16 bit long indices. */
        Short16,
        /** @brief 32 bit long indices. */
        Uint32,
    } value;

    [[nodiscard]] constexpr auto size_bytes() const -> usize {
        switch (value) {
            case Byte8: return 1;
            case Short16: return 2;
            case Uint32: return 4;
            default: UNREACHABLE;
        }
    }

    // no explicit by choice
    IndexFormat(const Enum value) : value(value) { }

    // no explicit by choice, conversion for switches
    constexpr operator Enum() const { return value; }
};

/** @brief Describes a @ref Buffer. Used for object creation via @ref Device. */
export struct BufferDescriptor {
    /** @brief An optional label. Mainly useful for debugging. */
    std::optional<std::string> label;
    /**
     * @brief Optional initial data. If present, performs a direct upload.
     * @note This is dropped by the @ref Buffer once owned by it. Therefore,
     * it is best to never access this via a @ref Buffer
     */
    std::optional<std::vector<u8>> data;
    /** @brief The initial size of the buffer. */
    usize size;
    /** @brief The intended use of the buffer. */
    BufferUsage usage;
};

/**
 * @brief The Buffer class represents a chunk of GPU memory.
 * This makes use of OpenGL DSA and can thus be used as a buffer
 * for arbitrary types of data (vertex buffers, index buffers etc...)
 */
export class Buffer final : public RenderResource<Buffer> {
public:
    using Base = RenderResource<Buffer>;

    /**
     * @brief Constructs a new Buffer.
     * @param device Pointer to the @ref Device.
     * @param handle @ref RenderResourceID of the @ref Buffer.
     */
    Buffer(
        Device* device,
        Handle handle
    );
    ~Buffer();

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    /** @brief Returns the descriptor of this Buffer. */
    [[nodiscard]] auto descriptor() const noexcept -> const BufferDescriptor&;
};

Buffer::Buffer(
    Device* device,
    const Handle handle
) : Base(device, handle) { }

Buffer::~Buffer() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_buffer(m_handle);
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : Base(std::move(other)) { }

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_buffer(m_handle);
        }

        Base::operator=(std::move(other));
    }
    return *this;
}

auto Buffer::descriptor() const noexcept -> const BufferDescriptor& { return m_device->buffer_descriptor(m_handle); }

} // namespace siren::render
