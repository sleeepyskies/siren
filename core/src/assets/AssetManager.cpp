#include "AssetManager.hpp"

namespace core::assets
{

static std::unordered_map<std::string, AssetType> extensionToType = {
    // shaders
    { ".frag", AssetType::SHADER },
    { ".fs", AssetType::SHADER },
    { ".vert", AssetType::SHADER },
    { ".vs", AssetType::SHADER },
    // model
    { ".gltf", AssetType::MODEL },
    { ".obj", AssetType::MODEL },
    //  mesh unsupported
    //  material unsupported
    // texture
    { ".png", AssetType::TEXTURE2D },
    { ".jpg", AssetType::TEXTURE2D },
    { ".jpeg", AssetType::TEXTURE2D },
};

// TODO: place "assets" into a configurable string constants rhing?
AssetManager::AssetManager(const fs::path& workingDirectory)
    : m_assetDirectory(workingDirectory / "assets"), m_registry(m_assetDirectory)
{
}

Ref<Asset> AssetManager::loadAsset(const AssetHandle& handle)
{
    // if loaded, return it
    if (m_registry.isLoaded(handle)) { return m_registry.getAsset(handle); }

    // iif not loaded but imported, load then update registry and return it
    if (m_registry.isImported(handle)) {
        const auto [path, type] = m_registry.getMetaData(handle);
        return importAssetByType(path, type);
    }

    // cannot load asset as no registry entry
    return nullptr;
}

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

Ref<Asset> AssetManager::importAssetByType(const fs::path& path, const AssetType type)
{
    // clean the path
    const fs::path path_ = getRelativePathTo(path, m_assetDirectory);

    Ref<Asset> asset = nullptr;

    switch (type) {
        case AssetType::NONE     : return nullptr;
        case AssetType::TEXTURE2D: {
            asset = TextureImporter::importTexture2D(path_);
            break;
        }
        case AssetType::MATERIAL: {
            wrn("Cannot import Materials as individual objects. Only in relation to models");
            break;
        }
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

const AssetRegistry& AssetManager::getAssetRegistry()
{
    return m_registry;
}

} // namespace core::assets
