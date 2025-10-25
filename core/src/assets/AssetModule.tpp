#pragma once

#include "geometry/primitive.hpp"

namespace siren::core
{

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

    // if not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto& [type, sourceData, creationType] = m_registry.getMetaData(handle);
        if (type == AssetType::NONE) {
            return nullptr;
        }
        // for imported assets, we have to re-import
        // for procedural assets, we have to regenerate
        // for sub imported assets, I have no clue :D
        switch (creationType) {
            case AssetMetaData::CreationType::IMPORTED: {
                const Ref<Asset>& asset = importAssetByType(std::get<Path>(sourceData), type);
                if (asset->getAssetType() == T::getStaticAssetType()) {
                    m_registry.updateAsset(handle, asset);
                    return std::static_pointer_cast<T>(asset);
                }
                wrn("Asset type mismatch when calling getAsset() on {}", asset);
                return nullptr;
            }
            case AssetMetaData::CreationType::PROCEDURAL: {
                const auto& mesh = generatePrimitive(std::get<PrimitiveParams>(sourceData));
                return mesh;
            }
            case AssetMetaData::CreationType::SUB_IMPORT: NotImplemented;
            default                                     : SirenAssert(false, "Invalid CreationType encountered.");
        }
    }

    // cannot load asset as no registry entry
    return nullptr;
}

} // namespace siren::core
