#pragma once

#include "Asset.hpp"
#include "utilities/spch.hpp"

namespace core::assets
{

class AssetRegistry
{
public:
    AssetRegistry();

    bool isLoaded(const AssetHandle& handle) const;
    bool registerAsset(const AssetHandle& handle, const Ref<Asset>& asset,
                       const AssetMetaData& metaData);
    bool unregisterAsset(const AssetHandle& handle);

    Ref<Asset> getAsset(const AssetHandle& handle) const;
    Maybe<AssetMetaData> getMetaData(const AssetHandle& handle) const;

private:
    struct RegistryEntry {
        Ref<Asset> asset = nullptr;
        AssetMetaData metaData;
    };

    std::unordered_map<AssetHandle, RegistryEntry> m_registry;
};

} // namespace core::assets
