/**
 * @file ImportContext.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "assets/AssetMetaData.hpp"

namespace siren::core
{
class AssetRegistry;

/**
 * @brief ImportContext is used by importers to load any sub-assets.
 */
class ImportContext
{
public:
    explicit ImportContext(AssetRegistry& registry);

    /// @brief Register an asset and return its handle.
    bool registerAsset(AssetHandle assetHandle, const Ref<Asset>& asset, const AssetMetaData& metaData) const;

private:
    AssetRegistry& m_registry;
};

} // namespace siren::core
