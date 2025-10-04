#include "AssetManager.hpp"

#include "ModelImporter.hpp"
#include "ShaderImporter.hpp"
#include "TextureImporter.hpp"

namespace core::assets
{

static std::unordered_map<std::string, AssetType> extensionToType = {
    // shaders
    { ".sshg", AssetType::SHADER },
    // model
    { ".gltf", AssetType::MODEL },
    { ".obj", AssetType::MODEL },
    //  mesh unsupported
    //  material unsupported
    // texture
    /*
    { ".png", AssetType::TEXTURE2D },
    { ".jpg", AssetType::TEXTURE2D },
    { ".jpeg", AssetType::TEXTURE2D },
    */
};

// TODO: place "assets" into a configurable string constants rhing?
AssetManager::AssetManager(const fs::path& workingDirectory)
    : m_assetDirectory(workingDirectory / "assets"), m_registry(m_assetDirectory)
{
}

// implemented in AssetManager.tpp
// Ref<Asset> AssetManager::getAsset(const AssetHandle& handle) const

Maybe<AssetHandle> AssetManager::importAsset(const fs::path& path)
{
    const std::string extension = path.extension().string();
    if (!extensionToType.contains(extension)) {
        wrn("Attempting to import an invalid asset at {}", path.string());
        return Nothing;
    }

    const AssetType type   = extensionToType[extension];
    const Ref<Asset> asset = importAssetByType(path, type);

    if (!asset) {
        wrn("Could not load asset from {}", path.string());
        return Nothing;
    }

    const AssetHandle handle{};

    if (!m_registry.registerAsset(handle, asset, path)) {
        wrn("Could not register asset from {}", path.string());
        return Nothing;
    }

    return handle;
}

Ref<Asset> AssetManager::importAssetByType(const fs::path& path, const AssetType type) const
{
    // path must either be absolute or relative to the assets dir
    fs::path path_;
    if (path.is_absolute()) {
        path_ = path;
    } else {
        path_ = m_assetDirectory / path;
    }

    Ref<Asset> asset = nullptr;

    switch (type) {
        case AssetType::NONE : return nullptr;
        /*
        case AssetType::TEXTURE2D: {
            asset = TextureImporter::importTexture2D(path_);
            break;
        }
        case AssetType::MATERIAL: {
            wrn("Cannot import Materials as individual objects. Only in relation to models");
            break;
        }
        */
        case AssetType::MODEL: {
            asset = ModelImporter::importModel(path_);
            break;
        }
        case AssetType::SHADER: {
            // TODO: how to handle shaders? since they are usually a bundled pipeline
            asset = ShaderImporter::importShader(path_);
            break;
        }
        case AssetType::SCENE: {
            // asset = SceneImporter::importScene(path_); TOOD
            break;
        }
    }

    return asset;
}

void AssetManager::unloadAsset(const AssetHandle& handle)
{
    // just unload from memory but keep in registry

    // no need to do anything
    if (!m_registry.isLoaded(handle)) { return; }

    m_registry.unloadAsset(handle);
}
void AssetManager::removeAsset(const AssetHandle& handle)
{
    // unload from memory and delete from registry

    // no need to do anything
    if (!m_registry.isLoaded(handle)) { return; }
    if (!m_registry.isImported(handle)) { return; }

    m_registry.removeAsset(handle);
}

const AssetRegistry& AssetManager::getAssetRegistry()
{
    return m_registry;
}

bool AssetManager::reloadAsset(const AssetHandle& handle)
{
    if (!m_registry.isImported(handle)) {
        wrn("Cannot reload Asset that is not imported");
        return false;
    }

    const AssetMetaData metaData = m_registry.getMetaData(handle);

    Ref<Asset> asset = importAssetByType(metaData.filePath, metaData.type);

    if (!asset) {
        wrn("Failed to reload Asset");
        return false;
    }

    m_registry.updateAsset(handle, asset);

    return true;
}

} // namespace core::assets
