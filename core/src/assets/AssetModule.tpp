#pragma once

#include "geometry/PrimitiveMesh.hpp"

namespace siren::core
{

template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetModule::getAsset(const AssetHandle& handle) const
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

    // if not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto [path, type, isVirtual] = m_registry.getMetaData(handle);
        const Ref<Asset>& asset            = importAssetByType(path, type);
        if (asset->getAssetType() == T::getStaticAssetType()) {
            return std::static_pointer_cast<T>(asset);
        }
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    // cannot load asset as no registry entry
    return nullptr;
}

} // namespace siren::core
