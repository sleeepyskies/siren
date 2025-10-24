/**
 * @file AssetMetaData.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

// todo: should we introduce parent/child tracking? Would be nice for recursive hot reloading...

/**
 * @brief Struct holding an Asset's Metadata.
 */
struct AssetMetaData {
    /// @brief Identifies the creation type of an Asset.
    enum class CreationType {
        IMPORTED,
        SUB_IMPORT,
        PROCEDURAL,
    };

    /// @brief The type of the asset
    AssetType type;
    /// @brief The source Path of the asset. Is not required.
    Maybe<Path> source;
    /// @brief The Asset's creation type.
    CreationType creationType;
};

} // namespace siren::core