#pragma once

#include "AssetRegistry.hpp"

namespace siren::assets
{

class AssetManager
{
public:
    explicit AssetManager(const fs::path& workingDirectory);
    ~AssetManager()                              = default;
    AssetManager(AssetManager&)                  = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> getAsset(const AssetHandle& handle) const;
    Maybe<AssetHandle> importAsset(const fs::path& path);
    void loadAsset(const AssetHandle& handle); // TODO:
    void unloadAsset(const AssetHandle& handle);
    void removeAsset(const AssetHandle& handle);
    bool reloadAsset(const AssetHandle& handle);

    AssetRegistry& getAssetRegistry();

private:
    fs::path m_assetDirectory{};
    AssetRegistry m_registry;

    Ref<Asset> importAssetByType(const fs::path& path, AssetType type) const;
};

} // namespace siren::assets

#include "assets/AssetManager.tpp" // hide templated function details
