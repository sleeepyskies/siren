/**
 * @file AssetMetaData.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "geometry/primitive.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

// todo: should we introduce parent/child tracking? Would be nice for recursive hot reloading...

using SourceData = std::variant<PrimitiveParams, Path>; // todo: some other source for sub-import?

/**
 * @brief Struct holding an Asset's Metadata.
 */
struct AssetMetaData {
    /// @brief Identifies the creation type of an Asset.
    enum class CreationType {
        IMPORTED, // SourceData must contain Path
        SUB_IMPORT,
        PROCEDURAL, // SourceData must contain PrimitiveParams
    };

    /// @brief The type of the asset
    AssetType type;
    /// @brief Information on how to re-construct this asset.
    SourceData sourceData;
    /// @brief The Asset's creation type.
    CreationType creationType;

    /// @brief Helper to get the path. Unsafe and caller must ensure the variant holds a @ref Path.
    Path getPath() const { return std::get<Path>(sourceData); }
    /// @brief Helper to get the path. Unsafe and caller must ensure the variant holds a @ref
    /// PrimitiveParams.
    PrimitiveParams getPrimitiveParams() const { return std::get<PrimitiveParams>(sourceData); }
};

} // namespace siren::core