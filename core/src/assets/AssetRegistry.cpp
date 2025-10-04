#include "AssetRegistry.hpp"

namespace core::assets
{
// TODO: Add logging to these functions

bool AssetRegistry::isLoaded(const AssetHandle& handle) const
{
    return m_loadedAssets.contains(handle); // we assume it has been imported if its loaded
}

bool AssetRegistry::isImported(const fs::path& path) const
{
    const fs::path path_ = getRelativePathTo(path, m_assetDirectory);
    return m_importedPaths.contains(path_);
}

bool AssetRegistry::isImported(const AssetHandle& handle) const
{
    return m_importedAssets.contains(handle);
}

bool AssetRegistry::registerAsset(const AssetHandle& handle, const Ref<Asset>& asset,
                                  const fs::path& path)
{
    const fs::path path_ = getRelativePathTo(path, m_assetDirectory);

    if (m_importedAssets.contains(handle) || m_loadedAssets.contains(handle) ||
        m_importedPaths.contains(path_)) {
        return false;
    }

    const AssetMetaData metaData{ path_, asset->getType() };

    m_loadedAssets[handle]   = asset;
    m_importedAssets[handle] = metaData;
    m_importedPaths.insert(path_);

    return true;
}

void AssetRegistry::removeAsset(const AssetHandle& handle)
{
    m_loadedAssets.erase(handle);

    if (!m_importedAssets.contains(handle)) { return; }

    const fs::path path = m_importedAssets[handle].filePath;
    m_importedPaths.erase(path);
    m_importedAssets.erase(handle);
}

bool AssetRegistry::updateAsset(const AssetHandle& handle, const Ref<Asset>& asset)
{
    if (!m_importedAssets.contains(handle)) { return false; }
    m_loadedAssets[handle] = asset;
    return true;
}

void AssetRegistry::unloadAsset(const AssetHandle& handle)
{
    m_loadedAssets.erase(handle);
}

Ref<Asset> AssetRegistry::getAsset(const AssetHandle& handle) const
{
    if (!m_loadedAssets.contains(handle)) { return nullptr; }
    return m_loadedAssets.at(handle);
}

AssetMetaData AssetRegistry::getMetaData(const AssetHandle& handle) const
{
    if (!m_importedAssets.contains(handle)) { return { "", AssetType::NONE }; }
    return m_importedAssets.at(handle);
}

} // namespace core::assets
