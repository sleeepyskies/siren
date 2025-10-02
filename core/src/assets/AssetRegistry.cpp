#include "AssetRegistry.hpp"

namespace core::assets
{
AssetRegistry::AssetRegistry()
{
}

bool AssetRegistry::isLoaded(const AssetHandle& handle) const
{
    return m_registry.contains(handle);
}

bool AssetRegistry::registerAsset(const AssetHandle& handle, const Ref<Asset>& asset,
                                  const AssetMetaData& metaData)
{
    if (m_registry.contains(handle)) return false;
    m_registry[handle] = { asset, metaData };
    return true;
}

bool AssetRegistry::unregisterAsset(const AssetHandle& handle)
{
    if (!m_registry.contains(handle)) return false;
    m_registry.erase(handle);
    return true;
}

Ref<Asset> AssetRegistry::getAsset(const AssetHandle& handle) const
{
    if (!m_registry.contains(handle)) return nullptr;
    return m_registry.at(handle).asset;
}

Maybe<AssetMetaData> AssetRegistry::getMetaData(const AssetHandle& handle) const
{
    if (!m_registry.contains(handle)) return Nothing;
    return m_registry.at(handle).metaData;
}

} // namespace core::assets
