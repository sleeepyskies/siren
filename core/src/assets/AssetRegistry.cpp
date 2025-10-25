#include "AssetRegistry.hpp"

#include "utilities/spch.hpp"

namespace siren::core
{

bool AssetRegistry::isLoaded(const AssetHandle& handle) const
{
    isLegalState(handle);
    return m_loadedAssets.contains(handle);
}

bool AssetRegistry::isImported(const AssetHandle& handle) const
{
    isLegalState(handle);
    return m_importedAssets.contains(handle);
}

bool AssetRegistry::registerAsset(const AssetHandle& handle,
                                  const Ref<Asset>& asset,
                                  const AssetMetaData& metaData)
{
    if (metaData.creationType == AssetMetaData::CreationType::IMPORTED) {
        if (!filesystem().exists(metaData.getPath())) {
            dbg("Could not register Imported Asset, as its Path does not exist.");
            return false;
        }
    }

    if (isLoaded(handle) || isImported(handle)) {
        dbg("Could not register Asset {} as its handle already exists.", handle);
        return false;
    }

    m_loadedAssets[handle]   = asset;
    m_importedAssets[handle] = metaData;

    isLegalState(handle);

    trc("Registered Asset {}", handle);
    return true;
}

void AssetRegistry::removeAsset(const AssetHandle& handle)
{
    m_loadedAssets.erase(handle);
    if (m_importedAssets.contains(handle)) {
        m_importedAssets.erase(handle);
    }

    isLegalState(handle);

    trc("Removed Asset {}", handle);
}

bool AssetRegistry::updateAsset(const AssetHandle& handle, const Ref<Asset>& asset)
{
    if (!isImported(handle)) {
        dbg("Could not update Asset {}", handle);
        return false;
    }

    m_loadedAssets[handle] = asset;
    isLegalState(handle);

    trc("Updated Asset {}", handle);
    return true;
}

void AssetRegistry::unloadAsset(const AssetHandle& handle)
{
    isLegalState(handle);
    if (!isLoaded(handle)) {
        return;
    }
    m_loadedAssets.erase(handle);
    isLegalState(handle);
    trc("Unloaded Asset {}", handle);
}

Ref<Asset> AssetRegistry::getAsset(const AssetHandle& handle) const
{
    isLegalState(handle);
    if (!m_loadedAssets.contains(handle)) {
        return nullptr;
    }
    return m_loadedAssets.at(handle);
}

AssetMetaData AssetRegistry::getMetaData(const AssetHandle& handle) const
{
    isLegalState(handle);
    if (!m_importedAssets.contains(handle)) {
        return {.type = AssetType::NONE};
    }
    return m_importedAssets.at(handle);
}

void AssetRegistry::isLegalState(const AssetHandle handle) const
{
    SirenAssert(m_loadedAssets.contains(handle) && m_importedAssets.contains(handle) ||
                    !m_loadedAssets.contains(handle) && m_importedAssets.contains(handle) ||
                    !m_loadedAssets.contains(handle) && !m_importedAssets.contains(handle),
                "Illegal AssetRegistry state for Asset {}!",
                handle);
}

} // namespace siren::core
