/**
 * @file AssetModule.hpp
 */
#pragma once

#include "AssetRegistry.hpp"
#include "ShaderCache.hpp"
#include "core/Module.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Primitive.hpp"

#include "renderer/material/MaterialKey.hpp"


namespace siren::core
{
// todo: rendermodule should probably take some respobsibility for gpu related objects, shader, texture etc

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
    bool Init() override;

    void Shutdown() override;

    const char* GetName() override { return "AssetModule"; }

    /// @brief Creates and returns a default standard @ref Material.
    AssetHandle createBasicMaterial(const std::string& name = "Basic Material");

    /// @brief Imports an Asset using a filepath. Returns AssetHandle::invalid() on error.
    AssetHandle importAsset(const Path& path);

    /// @brief Helper function that both imports and returns an asset from a filepath. Returns nullptr on error.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> importGetAsset(const Path& path);

    /// @brief Creates a PrimitiveMesh and returns its handle. Optionally takes the primitives
    /// constructor arguments, otherwise construct with default values.
    AssetHandle createPrimitive(const PrimitiveParams& primitiveParams);

    /// @brief Creates and returns a @ref Shader's AssetHandle for this @ref Material.
    AssetHandle createShader(const MaterialKey& materialKey) const;

    /// @brief Returns an Asset if loaded, nullptr otherwise.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> GetAsset(const AssetHandle& handle);

    /// @brief Returns the metadata associated with this handle. Read only.
    const AssetMetaData* getMetaData(AssetHandle handle) const;

    /// @brief Returns the metadata associated with this handle. Read and write.
    AssetMetaData* getMetaData(AssetHandle handle);

    /// @brief Unloads the Asset assigned to the given AssetHandle. Unload means to delete the
    /// Asset's data itself, but retain its meta-data.
    void unloadAsset(const AssetHandle& handle);

    /// @brief Removes the Asset assigned to the given AssetHandle. Remove means to delete both the
    /// Asset's data and meta-data.
    void removeAsset(const AssetHandle& handle);

    /// @brief Reloads the Asset tied to the given AssetHandle. Requires that the Asset has saved
    /// meta-data.
    bool reloadAsset(const AssetHandle& handle);

    /// @brief Reloads all the assets of the given type.
    bool reloadAssetType(AssetType type);

private:
    AssetRegistry m_registry{ };
    Own<ShaderCache> m_shaderCache = nullptr;

    Ref<Asset> importAssetByType(const Path& path, AssetType type);
    Ref<Mesh> GeneratePrimitive(const PrimitiveParams& params);

    // default assets
    AssetHandle m_checkerboardTexture = AssetHandle::invalid();
    AssetHandle m_grayTexture         = AssetHandle::invalid();
};
} // namespace siren::core

#include "assets/AssetModule.tpp" // hide templated function details
