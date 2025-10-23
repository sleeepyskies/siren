#pragma once

#include "geometry/primitives/PlaneModel.hpp"

namespace siren::assets
{

template <typename T>
    requires(std::derived_from<T, Asset>)
Ref<T> AssetManager::getAsset(const AssetHandle& handle) const
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

template <typename T>
    requires(std::derived_from<T, geometry::PrimitiveModel>)
AssetHandle AssetManager::createPrimitive()
{
    switch (T::getStaticPrimitiveType()) {
        case geometry::PrimitiveType::PLANE: {
            const AssetHandle handle{};
            const auto plane = makeRef<geometry::PlaneModel>();
            const Path path  = std::format("{}", handle); // todo: hack solution
            m_registry.registerAsset(handle, plane, path, true);
            return handle;
        }
        case geometry::PrimitiveType::CUBE   : NotImplemented;
        case geometry::PrimitiveType::CAPSULE: NotImplemented;
        default                              :;
    }

    SirenAssert(false, "Invalid PrimitiveType encountered");
}

} // namespace siren::assets
