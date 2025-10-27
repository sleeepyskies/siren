#include "ShaderCache.hpp"

#include "assets/AssetRegistry.hpp"
#include "assets/Asset.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "importers/ShaderImporter.hpp"
#include "renderer/shaders/Shader.hpp"

namespace siren::core
{

ShaderCache::ShaderCache(AssetRegistry& assetRegistry) : m_registry(assetRegistry)
{
}

Maybe<AssetHandle> ShaderCache::getOrCreate(const MaterialKey& key)
{
    if (m_cache.contains(key)) {
        return m_cache.at(key);
    }

    const Ref<Shader> shader = createVariant(key);
    const AssetHandle handle{};
    const AssetMetaData metData{
        .type = AssetType::SHADER,
        .sourceData = key,
        .creationType = AssetMetaData::CreationType::BUILTIN,
    };

    if (!m_registry.registerAsset(handle, shader, metData)) {
        return Nothing;
    }

    m_cache[key] = handle;
    return handle;
}

Ref<Shader> ShaderCache::createVariant(const MaterialKey& key) const
{
    // todo: once there are more shaders, we should use #defines to reduce uniforms passed, but for
    //  now with one shader only, its fine to just pass a uniform bitmask
    std::vector<std::string> defines{};

    Path path;
    switch (key.shadingMode) {
        case ShadingMode::LIT: {
            path = filesystem().getEngineRoot() / "assets/shaders/lit.sshg";
            break;
        }
        case ShadingMode::UNLIT: {
            path = filesystem().getEngineRoot() / "assets/shaders/unlit.sshg";
            break;
        }
        case ShadingMode::PBR: {
            path = filesystem().getEngineRoot() / "assets/shaders/pbr.sshg";
            break;
        }
    }

    const auto importer = ShaderImporter::create(path);
    return importer.load();
}


} // siren