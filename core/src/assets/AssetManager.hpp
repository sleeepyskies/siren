#pragma once

#include "AssetRegistry.hpp"

namespace siren::assets
{

class AssetManager
{
public:
    AssetManager()                               = default;
    ~AssetManager()                              = default;
    AssetManager(AssetManager&)                  = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> getAsset(const AssetHandle& handle) const;
    Maybe<AssetHandle> importAsset(const Path& path);
    void unloadAsset(const AssetHandle& handle);
    void removeAsset(const AssetHandle& handle);
    bool reloadAsset(const AssetHandle& handle);

    AssetRegistry& getAssetRegistry();

private:
    AssetRegistry m_registry{};

    Ref<Asset> importAssetByType(const Path& path, AssetType type) const;
};

} // namespace siren::assets

#include "assets/AssetManager.tpp" // hide templated function details
