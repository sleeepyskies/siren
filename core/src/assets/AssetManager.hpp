#pragma once

#include "AssetRegistry.hpp"

namespace core::assets
{

class AssetManager
{
public:
    explicit AssetManager(const fs::path& workingDirectory);
    ~AssetManager()                              = default;
    AssetManager(AssetManager&)                  = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    void init();
    void shutdown();

    template <typename A>
        requires(std::is_base_of_v<Asset, A>)
    Ref<A> getAsset(const AssetHandle& handle) const;
    Maybe<AssetHandle> importAsset(const fs::path& path);

    const AssetRegistry& getAssetRegistry();

private:
    fs::path m_assetDirectory{};
    AssetRegistry m_registry;

    Ref<Asset> importAssetByType(const fs::path& path, AssetType type) const;
};

} // namespace core::assets

#include "assets/AssetManager.tpp" // hide templated function details
