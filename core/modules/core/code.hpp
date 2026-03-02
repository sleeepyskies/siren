#pragma once

namespace siren::core
{

/**
 * @brief All siren error codes.
 */
enum class Code {
    None = 0, ///< @brief Nothing, shouldn't be used.

    // Various core systems
    ResourceLocked, ///< @brief Attempted to use a shared resource that is currently locked.
    LogicFail,      ///< @brief Some incorrect internal logic.
    IOFail,         ///< @brief Failure during IO.

    // Assets
    AssetNotFound = 100, ///< @brief Could not find the asset.
    AssetCorrupted,      ///< @brief The asset has invalid internal data.
    AssetUnsupported,    ///< @brief The asset is not supported by siren.

    // Rendering
    DeviceNotPresent, ///< @brief Attempted to perform some action with no device.

    MAX, ///< @brief Max value. Shouldn't be used.
};

} // namespace siren::core
