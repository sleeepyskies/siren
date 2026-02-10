#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

// todo: cache resources in the device? use handle as lookup key?

class Device;

/**
 * @brief Used to define strongly typed GpuResourceHandle.
 * @tparam Resource The GpuResource for which to define a Handle.
 */
template <typename Resource>
struct RenderResourceHandle {
    u32 value; ///< @brief The underlying backend specific Handle.

    /// @brief Constructs a new invalid handle;
    static auto invalid() -> RenderResourceHandle { return { 0 }; }
    /// @brief Checks if the handle is valid.
    [[nodiscard]] auto is_valid() const noexcept -> bool { return value != 0; }
    /// @brief Invalidates this handle.
    auto invalidate() noexcept -> void { value = 0; }
};

/**
 * @brief Simple base struct to identify render resources.
 * Furthermore, inheriting this class deletes any copy constructors.
 */
template <typename Resource>
struct RenderResource {
    friend class Device;

    RenderResource(Device* device, RenderResourceHandle<Resource> handle) : m_device(device), m_handle(handle) { }
    ~RenderResource()                                = default;
    RenderResource(const RenderResource&)            = delete;
    RenderResource& operator=(const RenderResource&) = delete;
    RenderResource(RenderResource&& other) noexcept
        : m_device(std::exchange(other.m_device, nullptr)), m_handle(std::exchange(other.m_handle, { 0 })) { }
    RenderResource& operator=(RenderResource&& other) noexcept {
        if (this != &other) {
            m_device = std::exchange(other.m_device, nullptr);
            m_handle = std::exchange(other.m_handle, { 0 });
        }
        return *this;
    }

    /// @brief Returns the underlying native handle for this resource.
    auto handle() const noexcept -> RenderResourceHandle<Resource> { return m_handle; }

protected:
    Device* m_device;                        ///< @brief Pointer to the render device.
    RenderResourceHandle<Resource> m_handle; ///< @brief Native handle for the underlying backend.
};

} // namespace siren::core
