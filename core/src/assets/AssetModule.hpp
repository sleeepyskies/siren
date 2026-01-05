/**
 * @file AssetModule.hpp
 */
#pragma once

#include "AssetRegistry.hpp"
#include "core/Module.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Primitive.hpp"


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
    bool Init() override;
    void Shutdown() override;

    const char* GetName() override { return "AssetModule"; }

    /// @brief Creates and returns a default standard @ref Material.
    AssetHandle CreateBasicMaterial(const std::string& name = "Basic Material");
    /// @brief Imports an Asset using a filepath. Returns AssetHandle::invalid() on error.
    AssetHandle Import(const Path& path);
    /// @brief Helper function that both imports and returns an asset from a filepath. Returns nullptr on error.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> ImportGet(const Path& path);
    /// @brief Creates a PrimitiveMesh and returns its handle. Optionally takes the primitives
    /// constructor arguments, otherwise construct with default values.
    AssetHandle CreatePrimitive(const PrimitiveParams& primitiveParams);
    /// @brief Returns an Asset if loaded, nullptr otherwise.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> GetAsset(const AssetHandle& handle);
    /// @brief Returns the metadata associated with this handle. Read only.
    const AssetMetaData* GetMetaData(AssetHandle handle) const;
    /// @brief Returns the metadata associated with this handle. Read and write.
    AssetMetaData* GetMetaData(AssetHandle handle);
    /// @brief Unloads the Asset assigned to the given AssetHandle. Unload means to delete the
    /// Asset's data itself, but retain its meta-data.
    void UnloadAsset(const AssetHandle& handle);
    /// @brief Removes the Asset assigned to the given AssetHandle. Remove means to delete both the
    /// Asset's data and meta-data.
    void RemoveAsset(const AssetHandle& handle);
    /// @brief Reloads the Asset tied to the given AssetHandle. Requires that the Asset has saved
    /// meta-data.
    bool ReloadAsset(const AssetHandle& handle);
    /// @brief Reloads all the assets of the given type.
    bool ReloadAssetType(AssetType type);

private:
    AssetRegistry m_registry{ };

    Ref<Asset> ImportAssetByType(const Path& path, AssetType type);
    Ref<Mesh> GeneratePrimitive(const PrimitiveParams& params);
};
} // namespace siren::core

#include "assets/AssetModule.tpp" // hide templated function details
