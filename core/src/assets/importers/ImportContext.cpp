#include "ImportContext.hpp"

#include "assets/AssetRegistry.hpp"

namespace siren::core
{
ImportContext::ImportContext(AssetRegistry& registry) : m_registry(registry) {}

bool ImportContext::registerAsset(const AssetHandle assetHandle,
                                  const Ref<Asset>& asset,
                                  const AssetMetaData& metaData) const
{
    return m_registry.registerAsset(assetHandle, asset, metaData);
}

} // namespace siren::core