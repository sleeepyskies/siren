#include "AssetModule.hpp"

#include "geometry/MeshGroup.hpp"
#include "importers/ModelImporter.hpp"
#include "importers/ShaderImporter.hpp"

namespace siren::core
{

static std::unordered_map<std::string, AssetType> extensionToType = {
    // shaders
    {".sshg", AssetType::SHADER},
    // model
    {".gltf", AssetType::MESH},
    {".obj", AssetType::MESH},
    //  material unsupported
    // texture
    /*
    { ".png", AssetType::TEXTURE2D },
    { ".jpg", AssetType::TEXTURE2D },
    { ".jpeg", AssetType::TEXTURE2D },
    */
};

Maybe<AssetHandle> AssetModule::importAsset(const Path& path)
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

AssetHandle AssetModule::createPrimitive(const PrimitiveParams& primitiveParams)
{
    Ref<MeshGroup> primitiveMesh = createRef<MeshGroup>(primitiveParams);
    AssetHandle handle{};
    m_registry.registerAsset(handle, primitiveMesh);
    return handle;
}

Ref<Asset> AssetModule::importAssetByType(const Path& path, const AssetType type) const
{
    const auto fsm = App::get().getFileSystemManager();

    const Path path_ = fsm.resolveVirtualPath(path);

    Ref<Asset> asset = nullptr;

    switch (type) {
        case AssetType::NONE    : return nullptr;
        /*
        case AssetType::TEXTURE2D: {
            asset = TextureImporter::importTexture2D(path_);
            break;
        }
        */
        case AssetType::MATERIAL: {
            NotImplemented;
            break;
        }
        case AssetType::MODEL: {
            asset = ModelImporter::importModel(path_);
            break;
        }
        case AssetType::SHADER: {
            asset = ShaderImporter::importShader(path_);
            break;
        }
        case AssetType::SCENE: {
            NotImplemented;
            break;
        }
        default: SirenAssert(false, "Invalid AssetType");
    }

    return asset;
}

void AssetModule::unloadAsset(const AssetHandle& handle)
{
    // no need to do anything
    if (!m_registry.isLoaded(handle)) {
        return;
    }

    m_registry.unloadAsset(handle);
}

void AssetModule::removeAsset(const AssetHandle& handle)
{
    // unload from memory and delete from registry

    // no need to do anything
    if (!m_registry.isLoaded(handle)) {
        return;
    }
    if (!m_registry.isImported(handle)) {
        return;
    }

    m_registry.removeAsset(handle);
}

bool AssetModule::reloadAsset(const AssetHandle& handle)
{
    if (!m_registry.isImported(handle)) {
        wrn("Cannot reload Asset that is not imported");
        return false;
    }

    const AssetMetaData metaData = m_registry.getMetaData(handle);

    const Ref<Asset> asset = importAssetByType(metaData.filePath, metaData.type);

    if (!asset) {
        wrn("Failed to reload Asset");
        return false;
    }

    m_registry.updateAsset(handle, asset);

    return true;
}

} // namespace siren::core
