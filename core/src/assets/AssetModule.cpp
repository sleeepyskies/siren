#include "AssetModule.hpp"

#include "filesystem/FileSystemModule.hpp"
#include "geometry/Mesh.hpp"
#include "importers/MeshImporter.hpp"
#include "importers/TextureImporter.hpp"

#include "renderer/RenderModule.hpp"
#include "renderer/material/Material.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{
// todo: finish asset classes here

static std::unordered_map<std::string, AssetType> extensionToType = {
    // shaders
    // { ".sshg", AssetType::Shader },
    // model
    { ".gltf", AssetType::Mesh },
    { ".obj", AssetType::Mesh },
    //  material unsupported
    // texture
    { ".png", AssetType::Texture2D },
    { ".jpg", AssetType::Texture2D },
    { ".jpeg", AssetType::Texture2D },
    // cube map
    { ".cube", AssetType::TextureCubeMap },
};

bool AssetModule::Init()
{
    return true;
}

void AssetModule::Shutdown()
{
    m_registry.clear();
}

AssetHandle AssetModule::createBasicMaterial(const std::string& name)
{
    const Ref<Material> material = CreateRef<Material>(name);
    const AssetHandle handle     = AssetHandle::create();
    const AssetMetaData metaData{ .type = AssetType::Material, .sourceData = material->getMaterialKey() };

    if (!m_registry.registerAsset(handle, material, metaData)) {
        return AssetHandle::invalid();
    }

    return handle;
}

AssetHandle AssetModule::importAsset(const Path& path)
{
    if (m_registry.isImported(path)) {
        return m_registry.getAssetHandle(path);
    }

    const std::string extension = path.extension().string();
    if (!extensionToType.contains(extension)) {
        wrn("Attempting to import an invalid asset at {}", path.string());
        return AssetHandle::invalid();
    }

    const Path path_ = filesystem().ResolveVirtualPath(path);

    const AssetType type   = extensionToType[extension];
    const Ref<Asset> asset = importAssetByType(path_, type);

    if (!asset) {
        wrn("Could not load asset from {}", path_.string());
        return AssetHandle::invalid();
    }

    const AssetHandle handle = AssetHandle::create();
    const AssetMetaData metaData{
        .type = type,
        .sourceData = path
    };

    if (!m_registry.registerAsset(handle, asset, metaData)) {
        wrn("Could not register asset from {}", path.string());
        return AssetHandle::invalid();
    }

    trc("Imported Asset {} from {}", handle, path_.string());
    return handle;
}

AssetHandle AssetModule::CreatePrimitive(const PrimitiveParams& primitiveParams)
{
    const Ref<Mesh> mesh = GeneratePrimitive(primitiveParams);

    const AssetHandle meshHandle = AssetHandle::create();
    const AssetMetaData meshMetaData{
        .type = AssetType::Mesh,
        .sourceData = primitiveParams,
    };

    if (!m_registry.registerAsset(meshHandle, mesh, meshMetaData)) {
        dbg("Could not create Primitive");
        return AssetHandle::invalid();
    }

    trc("Created Primitive {}", meshHandle);
    return meshHandle;
}

const AssetMetaData* AssetModule::getMetaData(const AssetHandle handle) const
{
    return m_registry.getMetaData(handle);
}

AssetMetaData* AssetModule::getMetaData(const AssetHandle handle)
{
    return m_registry.getMetaData(handle);
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

    const auto& [assetType, sourceData] = *m_registry.getMetaData(handle);

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
            return GeneratePrimitive(std::get<PrimitiveParams>(sourceData));
        }

        if constexpr (std::is_same_v<T, AssetHandle>) {
            wrn("Regenerating Sub Assets is not yet supported!");
            return nullptr;
        }

        wrn("Cannot regenerate Asset, invalid SourceData type.");
        return nullptr;
    };

    const Ref<Asset> asset = std::visit(visitor, m_registry.getMetaData(handle)->sourceData);

    m_registry.updateAsset(handle, asset);

    return true;
}

bool AssetModule::ReloadAssetType(const AssetType type)
{
    const bool res = m_registry.forEachLoaded(
        [this] (const auto& pair) -> bool {
            return reloadAsset(pair.first);
        },
        [type] (const auto& pair) -> bool {
            return pair.second->GetAssetType() == type;
        }
    );
    if (res) { nfo("Reloaded asset group!"); }
    return res;
}

Ref<Asset> AssetModule::importAssetByType(const Path& path, const AssetType type)
{
    Ref<Asset> asset;

    switch (type) {
        case AssetType::None: return nullptr;
        case AssetType::Material: {
            NotImplemented;
        }
        case AssetType::Mesh: {
            auto importer = MeshImporter::Create(path, ImportContext{ m_registry }).Defaults();
            asset         = importer.Load();
            break;
        }
        case AssetType::Texture2D: {
            auto importer = TextureImporter::create(path);
            asset         = importer.load2D();
            break;
        }
        case AssetType::TextureCubeMap: {
            auto importer = TextureImporter::create(path);
            asset         = importer.loadCubeMap();
            break;
        }
        default: SirenAssert(false, "Invalid AssetType");
    }

    return asset;
}

Ref<Mesh> AssetModule::GeneratePrimitive(const PrimitiveParams& params)
{
    const auto meshData = primitive::Generate(params, Renderer().GetPBRPipeline()->GetLayout());
    const auto mesh     = CreateRef<Mesh>(primitive::CreatePrimitiveName(params));
    const auto material = createBasicMaterial();
    if (!material || !mesh) { return nullptr; }

    mesh->AddSurface(
        {
            .transform = { 1 },
            .materialHandle = material,
            .vertices = meshData->vertices,
            .indices = meshData->indices,
            .indexCount = meshData->indexCount,
        }
    );

    return mesh;
}
} // namespace siren::core
