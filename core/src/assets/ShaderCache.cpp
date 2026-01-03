#include "ShaderCache.hpp"

#include "assets/Asset.hpp"
#include "assets/AssetRegistry.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "importers/ShaderImporter.hpp"
#include "renderer/shaders/Shader.hpp"


namespace siren::core
{
ShaderCache::ShaderCache(AssetRegistry& assetRegistry) : m_registry(assetRegistry) { }

AssetHandle ShaderCache::getOrCreate(const MaterialKey& key)
{
    if (m_cache.contains(key)) {
        return m_cache.at(key);
    }

    const auto& pair         = createVariant(key);
    const AssetHandle handle = AssetHandle::create();
    const AssetMetaData metaData{ .type = AssetType::Shader, .sourceData = pair.second };

    if (!m_registry.registerAsset(handle, pair.first, metaData)) {
        return AssetHandle::invalid();
    }

    m_cache[key] = handle;
    return handle;
}

std::pair<Ref<Shader>, Path> ShaderCache::createVariant(const MaterialKey& key) const
{
    // todo: once there are more shaders, we should use #defines to reduce uniforms passed, but for
    //  now with one shader only, its fine to just pass a uniform bitmask
    Vector<std::string> defines{ };

    Path path;
    switch (key.shadingMode) {
        case ShadingMode::Lit: {
            path = filesystem().getEngineRoot() / "assets/shaders/lit.sshg";
            break;
        }
        case ShadingMode::Unlit: {
            path = filesystem().getEngineRoot() / "assets/shaders/unlit.sshg";
            break;
        }
        case ShadingMode::PBR: {
            path = filesystem().getEngineRoot() / "assets/shaders/pbr.sshg";
            break;
        }
    }

    const auto importer = ShaderImporter::create(path);
    return { importer.load(), path };
}
} // namespace siren::core
