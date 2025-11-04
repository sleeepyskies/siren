/**
 * @file AssetRegistry.hpp
 */
#pragma once

#include "Asset.hpp"
#include "AssetMetaData.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @brief Handles checking whether assets have been imported or loaded, as well as retaining this
 * information. We define:
 *
 * - imported := We have knowledge about the asset, and it may or may not be loaded in memory.
 * - loaded := We have knowledge about the asset, and it is loaded into memory.
 */
class AssetRegistry
{
public:
    AssetRegistry()                                = default;
    ~AssetRegistry()                               = default;
    AssetRegistry(AssetRegistry&)                  = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;

    /// @brief Checks whether the asset associated with the given handle is currently loaded in
    /// memory.
    bool isLoaded(const AssetHandle& handle) const;
    /// @brief Checks whether the asset associated with the given handle has been imported yet.
    bool isImported(const AssetHandle& handle) const;
    /// @brief Checks whether the asset at this Path has been imported yet.
    bool isImported(const Path& path) const;

    /// @brief Save import and load data.
    bool registerAsset(
        const AssetHandle& handle,
        const Ref<Asset>& asset,
        const AssetMetaData& metaData
    );
    /// @brief Unloads the given asset. Meta-data is retained
    void unloadAsset(const AssetHandle& handle);
    /// @brief Unloads and un-imports the given asset
    void removeAsset(const AssetHandle& handle);
    /// @brief Updates the assets data, meta data remains the same.
    bool updateAsset(const AssetHandle& handle, const Ref<Asset>& asset);

    /// @brief Returns a Ref to the asset associated with the given handle
    Ref<Asset> getAsset(const AssetHandle& handle) const;
    /// @brief Returns a maybe AssetHandle for the asset at this path.
    AssetHandle getAssetHandle(const Path& path) const;
    /// @brief Returns the meta-data associated with the given handle
    AssetMetaData getMetaData(const AssetHandle& handle) const;

    /// @brief Completely resets state.
    void clear();

private:
    /// @brief All assets that are loaded in memory in siren internal format
    HashMap<AssetHandle, Ref<Asset>> m_loadedAssets{ };
    /// @brief All assets that have meta-data stored
    HashMap<AssetHandle, AssetMetaData> m_importedAssets{ };
    /// @brief A mapping of Path to AssetHandle. Used to quickly check if an asset already exists.
    HashMap<Path, AssetHandle> m_assetPaths{ };

    /// @brief Helper for debugging. Enforces invariants for the AssetRegistry's state.
    void isLegalState(AssetHandle handle) const;
};

} // namespace siren::core
