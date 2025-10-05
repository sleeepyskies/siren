#pragma once

namespace siren::assets
{

template <typename A>
    requires(std::is_base_of_v<Asset, A>)
Ref<A> AssetManager::getAsset(const AssetHandle& handle) const
{
    // if loaded, return it
    if (m_registry.isLoaded(handle)) {
        const Ref<Asset>& asset = m_registry.getAsset(handle);
        if (asset->getType() == A::getStaticType()) return std::static_pointer_cast<A>(asset);
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    // if not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto [path, type] = m_registry.getMetaData(handle);
        const Ref<Asset>& asset = importAssetByType(path, type);
        if (asset->getType() == A::getStaticType()) return std::static_pointer_cast<A>(asset);
        wrn("Asset type mismatch when calling getAsset() on {}", asset);
        return nullptr;
    }

    // cannot load asset as no registry entry
    return nullptr;
}

} // namespace core::assets
