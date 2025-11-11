#include "AssetRegistry.hpp"

#include "filesystem/FileSystemModule.hpp"
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

bool AssetRegistry::isImported(const Path& path) const
{
    return m_assetPaths.contains(path);
}

bool AssetRegistry::registerAsset(
    const AssetHandle& handle,
    const Ref<Asset>& asset,
    const AssetMetaData& metaData
)
{
    if (isLoaded(handle) || isImported(handle)) {
        dbg("Could not register Asset {} as its handle already exists.", handle);
        return false;
    }

    if (metaData.isImported() && !filesystem().exists(metaData.getPath())) {
        dbg("Could not register Imported Asset, as its Path does not exist.");
        return false;
    }

    m_loadedAssets[handle]   = asset;
    m_importedAssets[handle] = metaData;

    if (metaData.isImported()) {
        m_assetPaths[metaData.getPath()] = handle;
    }

    isLegalState(handle);

    trc("Registered Asset {}", handle);
    return true;
}

void AssetRegistry::removeAsset(const AssetHandle& handle)
{
    const auto& metaData = getMetaData(handle);

    if (m_loadedAssets.contains(handle)) {
        m_loadedAssets.erase(handle);
    }
    if (m_importedAssets.contains(handle)) {
        m_importedAssets.erase(handle);
    }

    if (metaData->isImported()) {
        m_assetPaths.erase(metaData->getPath());
    }

    isLegalState(handle);

    trc("Removed Asset {}", handle);
}

// fixme:
//  - this function has some flaws. it is possible to unload an asset, and then when updating
//  - change its type. We should probably template this function for compile time type safety.
bool AssetRegistry::updateAsset(const AssetHandle& handle, const Ref<Asset>& asset)
{
    // we do not allow changing the assets type
    if (m_loadedAssets.contains(handle)) {
        if (asset->getAssetType() != m_loadedAssets.at(handle)->getAssetType()) {
            dbg("Cannot update Asset {} due to an AssetType mismatch.", handle);
            return false;
        }
    }

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

AssetHandle AssetRegistry::getAssetHandle(const Path& path) const
{
    if (m_assetPaths.contains(path)) {
        return m_assetPaths.at(path);
    }
    return AssetHandle::invalid();
}

const AssetMetaData* AssetRegistry::getMetaData(const AssetHandle& handle) const
{
    isLegalState(handle);
    if (!m_importedAssets.contains(handle)) {
        return nullptr;
    }
    return &m_importedAssets.at(handle);
}

AssetMetaData* AssetRegistry::getMetaData(const AssetHandle& handle)
{
    isLegalState(handle);
    if (!m_importedAssets.contains(handle)) {
        return nullptr;
    }
    return &m_importedAssets.at(handle);
}

void AssetRegistry::clear()
{
    m_loadedAssets.clear();
    m_importedAssets.clear();
    m_assetPaths.clear();
}

void AssetRegistry::isLegalState(const AssetHandle handle) const
{
    const bool imported = m_importedAssets.contains(handle);
    const bool loaded   = m_loadedAssets.contains(handle);

    bool invariant = true;

    // loaded assets must be imported
    if (loaded && !imported) {
        invariant = false;
    }

    // imported assets must also have a path
    if (imported) {
        const auto& metaData = m_importedAssets.at(handle);
        if (metaData.isImported()) {
            invariant = invariant && m_assetPaths.contains(metaData.getPath());
        }
    }

    SirenAssert(invariant, "Illegal AssetRegistry state for Asset {}!", handle);
}

} // namespace siren::core
