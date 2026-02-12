#pragma once

#include "core/identifier_64.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{

// todo: cache resources in the device? use handle as lookup key?

class Device;

/**
 * @class RenderResourceID
 * @brief Defines a proxy handle to the underlying API's handle.
 * @tparam Resource The GpuResource for which to define a Handle.
 */
template <typename Resource>
class RenderResourceID final : Identifier64<RenderResourceID<Resource>> {
    /// @brief The kind of the resource
    using ResourceKind = Identifier64<RenderResourceID<Resource>>::Meta;
    // inherit ctors
    using Identifier64<RenderResourceID<Resource>>::Identifier64;
};

/**
 * @brief Simple base struct to identify render resources.
 * Enforces disabling copies on all RenderResource's, as well
 * as provides access to the Device.
 */
template <typename Resource>
struct RenderResource {
    friend class Device;

    RenderResource(Device* device, RenderResourceID<Resource> handle) : m_device(device), m_handle(handle) { }
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
    auto handle() const noexcept -> RenderResourceID<Resource> { return m_handle; }

protected:
    Device* m_device;                    ///< @brief Pointer to the render device.
    RenderResourceID<Resource> m_handle; ///< @brief Native handle for the underlying backend.
};

} // namespace siren::core
