/**
 * @file AssetModule.hpp
 */
#pragma once

#include "AssetRegistry.hpp"
#include "geometry/PrimitiveMesh.hpp"

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
    bool initialize() override { return true; }

    void shutdown() override {}

    const char* getName() override { return "AssetModule"; }

    /// @brief Imports an Asset using a filepath. Returns Nothing on error.
    Maybe<AssetHandle> importAsset(const Path& path);

    /// @brief Creates a PrimitiveMesh and returns its handle. Optionally takes the primitives
    /// constructor arguments, otherwise construct with default values.
    AssetHandle createPrimitive(const PrimitiveParams& primitiveParams) const;

    /// @brief Returns an Asset if loaded, nullptr otherwise.
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> getAsset(const AssetHandle& handle) const;

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

    Ref<Asset> importAssetByType(const Path& path, AssetType type) const;
};

} // namespace siren::core

#include "assets/AssetModule.tpp" // hide templated function details
