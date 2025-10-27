/**
 * @file ShaderCache.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "renderer/shaders/Shader.hpp"

namespace siren::core
{
struct MaterialKey;
class AssetRegistry;

/**
 * @brief The ShaderCache handles caching and generating shaders based on @ref MaterialKey.
 */
class ShaderCache
{
public:
    explicit ShaderCache(AssetRegistry& assetRegistry);

    Maybe<AssetHandle> getOrCreate(const MaterialKey& key);

private:
    HashMap<MaterialKey, AssetHandle> m_cache{};
    AssetRegistry& m_registry;

    Ref<Shader> createVariant(const MaterialKey& key) const;
};
} // namespace siren::core
