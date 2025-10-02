#include "AssetRegistry.hpp"

namespace core::assets
{

bool AssetRegistry::isLoaded(const AssetHandle& handle) const
{
    return m_loadedAssets.contains(handle); // we assume it has been imported if its loaded
}

bool AssetRegistry::isImported(const fs::path& path) const
{
    fs::path resolved = path.is_absolute() ? path : resolved / m_assetDirectory / path;
    resolved          = resolved.lexically_normal();
    return m_importedPaths.contains(resolved);
}

bool AssetRegistry::isImported(const AssetHandle& handle) const
{
    return m_importedAssets.contains(handle);
}

bool AssetRegistry::registerAsset(const AssetHandle& handle, const Ref<Asset>& asset,
                                  const fs::path& path)
{
    fs::path path_ = path;
    path_          = path_.lexically_relative(m_assetDirectory);
    path_          = path_.lexically_normal();

    if (m_importedAssets.contains(handle) || m_loadedAssets.contains(handle) ||
        m_importedPaths.contains(path_)) {
        return false;
    }

    const AssetMetaData metaData{ path_, asset->getAssetType() };

    m_loadedAssets[handle]   = asset;
    m_importedAssets[handle] = metaData;
    m_importedPaths.insert(path_);

    return true;
}

void AssetRegistry::unregisterAsset(const AssetHandle& handle)
{
    m_loadedAssets.erase(handle);

    if (!m_importedAssets.contains(handle)) return;

    const fs::path path = m_importedAssets[handle].filePath;
    m_importedPaths.erase(path);
    m_importedAssets.erase(handle);
}

void AssetRegistry::unloadAsset(const AssetHandle& handle)
{
    m_loadedAssets.erase(handle);
}

Ref<Asset> AssetRegistry::getAsset(const AssetHandle& handle) const
{
    if (!m_loadedAssets.contains(handle)) return nullptr;
    return m_loadedAssets.at(handle);
}

Maybe<AssetMetaData> AssetRegistry::getMetaData(const AssetHandle& handle) const
{
    if (!m_importedAssets.contains(handle)) return Nothing;
    return m_importedAssets.at(handle);
}

} // namespace core::assets
