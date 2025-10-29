#include "AssetModule.hpp"

#include "filesystem/FileSystemModule.hpp"
#include "geometry/Mesh.hpp"
#include "importers/MeshImporter.hpp"
#include "importers/ShaderImporter.hpp"
#include "importers/TextureImporter.hpp"

#include "renderer/material/Material.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{

// todo: finish asset classes here

static std::unordered_map<std::string, AssetType> extensionToType = {
    // shaders
    { ".sshg", AssetType::SHADER },
    // model
    { ".gltf", AssetType::MESH },
    { ".obj", AssetType::MESH },
    //  material unsupported
    // texture
    { ".png", AssetType::TEXTURE2D },
    { ".jpg", AssetType::TEXTURE2D },
    { ".jpeg", AssetType::TEXTURE2D },
};

bool AssetModule::initialize()
{
    m_shaderCache = createOwn<ShaderCache>(m_registry);

    // todo: default assets

    return true;
}

void AssetModule::shutdown()
{
    m_registry.clear();
}

Maybe<AssetHandle> AssetModule::createBasicMaterial()
{
    const Ref<Material> material = createRef<Material>("Default Material");
    const auto shaderHandle      = createShader(material->getMaterialKey());
    if (!shaderHandle) {
        return Nothing;
    }
    material->shaderHandle   = *shaderHandle;
    const AssetHandle handle = AssetHandle::create();
    const AssetMetaData metaData{ .type = AssetType::MATERIAL, .sourceData = std::monostate{ } };

    if (!m_registry.registerAsset(handle, material, metaData)) {
        return Nothing;
    }

    return handle;
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

    const AssetHandle handle = AssetHandle::create();
    const AssetMetaData metaData{
        .type = type,
        .sourceData = path
    };

    if (!m_registry.registerAsset(handle, asset, metaData)) {
        wrn("Could not register asset from {}", path.string());
        return Nothing;
    }

    trc("Imported Asset {} from {}", handle, path_.string());
    return handle;
}

Maybe<AssetHandle> AssetModule::createPrimitive(const PrimitiveParams& primitiveParams)
{
    const Ref<Mesh> mesh = generatePrimitive(primitiveParams);

    const AssetHandle meshHandle = AssetHandle::create();
    const AssetMetaData meshMetaData{
        .type = AssetType::MESH,
        .sourceData = primitiveParams,
    };

    if (!m_registry.registerAsset(meshHandle, mesh, meshMetaData)) {
        dbg("Could not create Primitive");
        return Nothing;
    }

    trc("Created Primitive {}", meshHandle);
    return meshHandle;
}

Maybe<AssetHandle> AssetModule::createShader(const MaterialKey& materialKey) const
{
    return m_shaderCache->getOrCreate(materialKey);
}

AssetHandle AssetModule::clone(const AssetHandle handle)
{
    const AssetHandle clonedHandle = AssetHandle::create();
    const AssetMetaData metaData   = m_registry.getMetaData(handle);
    const auto asset               = m_registry.getAsset(handle);
    m_registry.registerAsset(clonedHandle, asset, metaData);
    trc("Cloned Asset: {}. New Asset: {}", handle, clonedHandle);
    return clonedHandle;
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

    const auto& [assetType, sourceData] = m_registry.getMetaData(handle);

    auto visitor = [&]<typename TArg> (TArg&&)-> Ref<Asset> {
        using T = std::decay_t<TArg>;

        if (std::is_same_v<T, std::monostate>) {
            wrn("Cannot regenerate Asset with no SourceData.");
            return nullptr;
        }

        if constexpr (std::is_same_v<T, Path>) {
            return importAssetByType(std::get<Path>(sourceData), assetType);
        }

        if constexpr (std::is_same_v<T, PrimitiveParams>) {
            return generatePrimitive(std::get<PrimitiveParams>(sourceData));
        }

        if constexpr (std::is_same_v<T, AssetHandle>) {
            wrn("Regenerating Sub Assets is not yet supported!");
            return nullptr;
        }

        if constexpr (std::is_same_v<T, MaterialKey>) {
            const auto res = m_shaderCache->getOrCreate(std::get<MaterialKey>(sourceData));
            if (!res) {
                wrn("Regenerating Shader Variant failed.");
                return nullptr;
            }
            return m_registry.getAsset(*res);
        }

        wrn("Cannot regenerate Asset, invalid SourceData type.");
        return nullptr;
    };

    const Ref<Asset> asset = std::visit(visitor, m_registry.getMetaData(handle).sourceData);

    m_registry.updateAsset(handle, asset);

    return true;
}

Ref<Asset> AssetModule::importAssetByType(const Path& path, const AssetType type)
{
    Ref<Asset> asset = nullptr;

    switch (type) {
        case AssetType::NONE: return nullptr;
        case AssetType::MATERIAL: {
            NotImplemented;
        }
        case AssetType::MESH: {
            auto importer = MeshImporter::create(path, ImportContext{ m_registry }).defaults();
            asset         = importer.load();
            break;
        }
        case AssetType::SHADER: {
            const auto importer = ShaderImporter::create(path);
            asset               = importer.load();
            break;
        }
        case AssetType::TEXTURE2D: {
            auto importer = TextureImporter::create(path);
            asset         = importer.load();
            break;
        }
        default: SirenAssert(false, "Invalid AssetType");
    }

    return asset;
}

Ref<Mesh> AssetModule::generatePrimitive(const PrimitiveParams& params)
{
    const auto& vertexArray = primitive::generate(params);
    const auto mesh         = createRef<Mesh>(primitive::createPrimitiveName(params));
    const auto material     = createBasicMaterial();
    if (!material) { return nullptr; }
    mesh->emplaceSurface(glm::mat4{ 1 }, *material, vertexArray);
    return mesh;
}

} // namespace siren::core
