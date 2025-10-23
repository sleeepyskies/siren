#pragma once

#include "AssetRegistry.hpp"

namespace siren::assets
{

// The desired API would be something like this, assets is a central place to
///   const app&    = core::App::get();
//    const assets& = core::App::assets();
//    const AssetHandle mesh = assets.load<ArrayMesh>("some/path.mesh");
//    const AssetHandle capsule = assets.create<Capsule3D>();

class Assets
{
};

class AssetManager
{
public:
    AssetManager()                               = default;
    ~AssetManager()                              = default;
    AssetManager(AssetManager&)                  = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    Maybe<AssetHandle> importAsset(const Path& path);
    template <typename T>
        requires(std::derived_from<T, geometry::PrimitiveModel>)
    AssetHandle createPrimitive(); // todo: pass Args here?
    template <typename T>
        requires(std::derived_from<T, Asset>)
    Ref<T> getAsset(const AssetHandle& handle) const;
    void unloadAsset(const AssetHandle& handle);
    void removeAsset(const AssetHandle& handle);
    bool reloadAsset(const AssetHandle& handle);

private:
    AssetRegistry m_registry{};

    Ref<Asset> importAssetByType(const Path& path, AssetType type) const;
};

} // namespace siren::assets

#include "assets/AssetManager.tpp" // hide templated function details
