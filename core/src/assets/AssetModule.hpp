/**
 * @file AssetModule.hpp
 */
#pragma once

#include "AssetRegistry.hpp"
#include "ShaderCache.hpp"
#include "core/Module.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/primitive.hpp"
#include "renderer/material/MaterialKey.hpp"

namespace siren::core
{

/**
 * @brief The AssetModule is responsible for importing, creating, caching and managing the lifetime
 * of Assets in siren.
 * Systems should prefer to store just an AssetHandle over a Ref<Asset>, and
 * then query the AssetManager for the desired Asset on the fly.
 */
class AssetModule final : public Module
{
public:
    /// @brief Loads all default assets.
    bool initialize() override;

    void shutdown() override;

    const char* getName() override { return "AssetModule"; }

    /// @brief Returns a copy of the basic base @ref Material.
    AssetHandle createBasicMaterial();

    /// @brief Returns the standard PBR shader.
    AssetHandle getBasicShader();

    /// @brief Imports an Asset using a filepath. Returns Nothing on error.
    Maybe<AssetHandle> importAsset(const Path& path);

    /// @brief Creates a PrimitiveMesh and returns its handle. Optionally takes the primitives
    /// constructor arguments, otherwise construct with default values.
    Maybe<AssetHandle> createPrimitive(const PrimitiveParams& primitiveParams);

    /// @brief Creates and returns a @ref Shader's AssetHandle for this @ref Material.
    Maybe<AssetHandle> createShader(const MaterialKey& materialKey) const;

    /// @brief Creates a copy of the @ref Asset with this handle, and returns the new @ref
    /// AssetHandle.
    AssetHandle clone(AssetHandle handle);

    /// @brief Returns an Asset if loaded, nullptr otherwise.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> getAsset(const AssetHandle& handle);

    /// @brief Unloads the Asset assigned to the given AssetHandle. Unload means to delete the
    /// Asset's data itself, but retain its meta-data.
    void unloadAsset(const AssetHandle& handle);

    /// @brief Removes the Asset assigned to the given AssetHandle. Remove means to delete both the
    /// Asset's data and meta-data.
    void removeAsset(const AssetHandle& handle);

    /// @brief Reloads the Asset tied to the given AssetHandle. Requires that the Asset has saved
    /// meta-data.
    bool reloadAsset(const AssetHandle& handle);

private:
    AssetRegistry m_registry{};
    Own<ShaderCache> m_shaderCache = nullptr;

    Ref<Asset> importAssetByType(const Path& path, AssetType type);
    Ref<Mesh> generatePrimitive(const PrimitiveParams& params);

    // default materials
};

} // namespace siren::core

#include "assets/AssetModule.tpp" // hide templated function details
