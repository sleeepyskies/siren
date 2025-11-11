#pragma once

#include "assets/Asset.hpp"


namespace siren::core
{
template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetModule::importGetAsset(const Path& path)
{
    const AssetHandle handle = importAsset(path);
    if (!handle) { return nullptr; }
    return getAsset<T>(handle);
}

template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetModule::getAsset(const AssetHandle& handle)
{
    if (!handle) {
        wrn("Cannot getAsset from invalid AssetHandle.");
        return nullptr;
    }

    // if loaded, return it
    if (m_registry.isLoaded(handle)) {
        const Ref<Asset>& asset = m_registry.getAsset(handle);
        if (asset->getAssetType() == T::getStaticAssetType()) {
            return std::static_pointer_cast<T>(asset);
        }
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    if (reloadAsset(handle)) {
        return std::static_pointer_cast<T>(m_registry.getAsset(handle));
    }

    // if not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto& [type, sourceData] = *m_registry.getMetaData(handle);
        if (type == AssetType::NONE) {
            return nullptr;
        }
    }

    // cannot load asset as no registry entry
    wrn("Cannot get asset, AssetHandle points to nothing.");
    return nullptr;
}

template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetModule::getFallback()
{
    const AssetType type = T::getStaticAssetType();
    if (m_fallbackAssets.contains(type)) {
        return m_fallbackAssets[type];
    }
    return nullptr;
}

} // namespace siren::core
