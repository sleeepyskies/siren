#pragma once

#include "core/Core.hpp"


namespace siren::core
{

/**
 * @brief Simple base struct to identify GpuResources.
 * Furthermore, inheriting this class deletes any copy constructors.
 */
struct GpuResource {
    GpuResource()                              = default;
    GpuResource(const GpuResource&)            = delete;
    GpuResource& operator=(const GpuResource&) = delete;
    virtual ~GpuResource() = 0;
};

/**
 * @brief Used to define strongly typed GpuResourceHandle.
 * @tparam Resource The GpuResource for which to define a Handle.
 */
template <typename Resource>
struct GpuResourceHandle {
    u32 value; ///< @brief The underlying backend specific Handle.
    static auto invalid() -> GpuResourceHandle<Resource> { return { 0 }; }
};
} // namespace siren::core
