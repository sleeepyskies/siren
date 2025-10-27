/**
 * @file AssetMetaData.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "geometry/primitive.hpp"
#include "renderer/material/MaterialKey.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

// todo: should we introduce parent/child tracking? Would be nice for recursive hot reloading...
// fixme: PrimitveParams are out of sync if we make any modifications after construction.

/**
 * @brief The source of this Asset. May either be a filepath, the parameters used to generate this asset, or its parent asset.
 */
using SourceData = std::variant<PrimitiveParams, Path, AssetHandle, MaterialKey>;

/**
 * @brief Struct holding an Asset's Metadata.
 */
struct AssetMetaData
{
    /// @brief Identifies the creation type of an Asset.
    enum class CreationType
    {
        IMPORTED,   ///< SourceData must contain Path
        SUB_IMPORT, ///< Sub assets from files, such as materials in mesh files
        PROCEDURAL, ///< SourceData must contain PrimitiveParams
        BUILTIN,    ///< used for builtin assets such as stock shaders
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