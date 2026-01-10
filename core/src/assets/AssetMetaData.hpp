/**
 * @file AssetMetaData.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "geometry/Primitive.hpp"
#include "renderer/material/MaterialKey.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
// todo: should we introduce parent/child tracking? Would be nice for recursive hot reloading...
// fixme: PrimitiveParams are out of sync if we make any modifications after construction.

/**
 * @brief Struct holding an Asset's Metadata.
 */
struct AssetMetaData
{
    /**
     * @brief Information on how to reconstruct this asset.
     */
    using SourceData = std::variant<
        std::monostate,  // no source
        Path,            // imported from file
        PrimitiveParams, // generated from parameters
        AssetHandle      // sub asset of another asset
    >;

    /// @brief The type of the asset
    AssetType type;
    /// @brief Information on how to re-construct this asset.
    SourceData sourceData;

    bool isImported() const { return std::holds_alternative<Path>(sourceData); }

    bool isProcedural() const { return std::holds_alternative<PrimitiveParams>(sourceData); }

    bool isSubImport() const { return std::holds_alternative<AssetHandle>(sourceData); }

    bool isShaderVariant() const { return std::holds_alternative<MaterialKey>(sourceData); }

    MaterialKey getMaterialKey() const { return std::get<MaterialKey>(sourceData); }

    Path getPath() const { return std::get<Path>(sourceData); }

    PrimitiveParams& getPrimitiveParams() { return std::get<PrimitiveParams>(sourceData); }

    AssetHandle getAssetHandle() const { return std::get<AssetHandle>(sourceData); }
};
} // namespace siren::core
