#pragma once

#include "Asset.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
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
    explicit AssetRegistry(const fs::path& assetDirectory) : m_assetDirectory(assetDirectory)
    {
    }
    ~AssetRegistry()                               = default;
    AssetRegistry(AssetRegistry&)                  = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;

    /// @brief Checks whether the asset associated with the given handle is currently loaded in
    /// memory
    bool isLoaded(const AssetHandle& handle) const;
    /// @brief Checks whether the asset at the given path has been imported yet. Can handle absolute
    /// and relative paths to assets/
    bool isImported(const fs::path& path) const;
    /// @brief Checks whether the asset associated with the given handle has been imported yet.
    bool isImported(const AssetHandle& handle) const;

    /// @brief Save import and load data. A virtual asset is one that does not have its own file,
    /// and is contained within another asset file
    bool registerAsset(const AssetHandle& handle, const Ref<Asset>& asset, const fs::path& path,
                       bool isVirtualAsset = false);
    /// @brief Unloads the given asset. Meta-data is retained
    void unloadAsset(const AssetHandle& handle);
    /// @brief Unloads and un-imports the given asset
    void removeAsset(const AssetHandle& handle);
    /// @brief Updates the assets data
    bool updateAsset(const AssetHandle& handle, const Ref<Asset>& asset);

    /// @brief Returns a Ref to the asset associated with the given handle
    Ref<Asset> getAsset(const AssetHandle& handle) const;
    /// @brief Returns the meta-data associated with the given handle
    AssetMetaData getMetaData(const AssetHandle& handle) const;

private:
    /// @brief The absolute path to the asset base directory
    fs::path m_assetDirectory;

    /// @brief All assets that are loaded in memory in siren internal format
    std::unordered_map<AssetHandle, Ref<Asset>> m_loadedAssets;
    /// @brief All assets that have meta-data stored
    std::unordered_map<AssetHandle, AssetMetaData> m_importedAssets;
    /// @brief All assets that have meta-data stored
    std::unordered_set<fs::path> m_importedPaths;
};

} // namespace siren::assets
