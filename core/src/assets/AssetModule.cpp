#include "AssetModule.hpp"

#include "filesystem/FileSystemModule.hpp"
#include "geometry/Mesh.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

// todo: finish asset classes here

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

bool AssetModule::initialize()
{
    NotImplemented;
}

Maybe<AssetHandle> AssetModule::importAsset(const Path& path)
{
    if (m_registry.isImported(path)) {
        return m_registry.getAssetHandle(path);
    }

    const std::string extension = path.extension().string();
    if (!extensionToType.contains(extension)) {
        wrn("Attempting to import an invalid asset at {}", path.string());
        return Nothing;
    }

    const Path path_ = filesystem().resolveVirtualPath(path);

    const AssetType type   = extensionToType[extension];
    const Ref<Asset> asset = importAssetByType(path_, type);

    if (!asset) {
        wrn("Could not load asset from {}", path_.string());
        return Nothing;
    }

    const AssetHandle handle{};
    const AssetMetaData metaData{
        .type         = type,
        .sourceData   = path,
        .creationType = AssetMetaData::CreationType::IMPORTED,
    };

    if (!m_registry.registerAsset(handle, asset, metaData)) {
        wrn("Could not register asset from {}", path.string());
        return Nothing;
    }

    trc("Imported Asset {} from {}", handle, path_.string());
    return handle;
}

AssetHandle AssetModule::clone(const AssetHandle handle)
{
    const AssetHandle clonedHandle{};
    const AssetMetaData metaData = m_registry.getMetaData(handle);
    const auto asset             = m_registry.getAsset(handle);
    m_registry.registerAsset(clonedHandle, asset, metaData);
    trc("Cloned Asset: {}. New Asset: {}", handle, clonedHandle);
    return clonedHandle;
}

Maybe<AssetHandle> AssetModule::createPrimitive(const PrimitiveParams& primitiveParams)
{
    const Ref<Mesh> mesh = generatePrimitive(primitiveParams);

    const AssetHandle meshHandle{};
    const AssetMetaData meshMetaData{.type         = AssetType::MESH,
                                     .sourceData   = primitiveParams,
                                     .creationType = AssetMetaData::CreationType::PROCEDURAL};

    if (!m_registry.registerAsset(meshHandle, mesh, meshMetaData)) {
        dbg("Could not create Primitive");
        return Nothing;
    }

    dbg("Created Primitive {}", meshHandle);
    return meshHandle;
}

Ref<Asset> AssetModule::importAssetByType(const Path& path, const AssetType type) const
{
    Ref<Asset> asset = nullptr;

    switch (type) {
        case AssetType::NONE    : return nullptr;
        case AssetType::MATERIAL: {
            NotImplemented;
            break;
        }
        case AssetType::MESH: {
            asset = MeshImporter::importMesh(path);
            break;
        }
        case AssetType::SHADER: {
            asset = ShaderImporter::importShader(path);
            break;
        }
        default: SirenAssert(false, "Invalid AssetType");
    }

    return asset;
}

void AssetModule::unloadAsset(const AssetHandle& handle)
{
    m_registry.unloadAsset(handle);
}

void AssetModule::removeAsset(const AssetHandle& handle)
{
    m_registry.removeAsset(handle);
}

bool AssetModule::reloadAsset(const AssetHandle& handle)
{
    if (!m_registry.isImported(handle)) {
        wrn("Cannot reload Asset that is not imported");
        return false;
    }

    const AssetMetaData metaData = m_registry.getMetaData(handle);

    Ref<Asset> asset;
    switch (metaData.creationType) {
        case AssetMetaData::CreationType::IMPORTED: {
            const Path path = filesystem().resolveVirtualPath(metaData.getPath());
            asset           = importAssetByType(path, metaData.type);
            break;
        }
        case AssetMetaData::CreationType::PROCEDURAL: {
            asset = generatePrimitive(metaData.getPrimitiveParams());
            break;
        };
        case AssetMetaData::CreationType::SUB_IMPORT: NotImplemented;
    }

    if (!asset) {
        wrn("Failed to reload Asset");
        return false;
    }

    m_registry.updateAsset(handle, asset);

    return true;
}

Ref<Mesh> AssetModule::generatePrimitive(const PrimitiveParams& params)
{
    const auto& vertexArray    = primitive::generate(params);
    const auto mesh            = createRef<Mesh>(primitive::createPrimitiveName(params));
    const AssetHandle material = createBasicMaterial();
    mesh->addSurface(
        {.m_transform = {1}, .m_materialHandle = material, .m_vertexArray = vertexArray});
    return mesh;
}

} // namespace siren::core
