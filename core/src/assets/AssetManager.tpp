#pragma once

namespace siren::assets
{

template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetManager::getAsset(const AssetHandle& handle) const
{
    // if loaded, return it
    if (m_registry.isLoaded(handle)) {
        const Ref<Asset>& asset = m_registry.getAsset(handle);
        if (asset->getType() == T::getStaticType()) { return std::static_pointer_cast<T>(asset); }
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    // if not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto [path, type, isVirtual] = m_registry.getMetaData(handle);
        const Ref<Asset>& asset            = importAssetByType(path, type);
        if (asset->getType() == T::getStaticType()) { return std::static_pointer_cast<T>(asset); }
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    // cannot load asset as no registry entry
    return nullptr;
}

} // namespace siren::assets
