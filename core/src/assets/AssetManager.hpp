#pragma once

#include "AssetRegistry.hpp"
#include "geometry/Mesh.hpp"
#include "utilities/spch.hpp"

#include "ModelImporter.hpp"
#include "SceneImporter.hpp"
#include "ShaderImporter.hpp"
#include "TextureImporter.hpp"

namespace core::assets
{

class AssetManager
{
public:
    explicit AssetManager(const fs::path& workingDirectory);
    ~AssetManager()                              = default;
    AssetManager(AssetManager&)                  = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    Ref<Asset> loadAsset(const AssetHandle& handle);
    Maybe<AssetHandle> importAsset(const fs::path& path);

    const AssetRegistry& getAssetRegistry();

private:
    fs::path m_assetDirectory;
    AssetRegistry m_registry;

    Ref<Asset> importAssetByType(const fs::path& path, AssetType type);
};

} // namespace core::assets
