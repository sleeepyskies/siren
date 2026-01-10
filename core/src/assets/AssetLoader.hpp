/**
 * @file AssetLoader.hpp
 * @brief Home for the definitions of all asset loaders.
 */
#pragma once

#include "core/FileSystem.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/Texture.hpp"
#include "renderer/shaders/Shader.hpp"
#include "core/TypeName.hpp"


namespace siren::core
{
class AssetServer;

class AssetLoaderBase
{
public:
    virtual ~AssetLoaderBase() = default;
};

template <typename A>
class AssetLoader final : public AssetLoaderBase
{
    A* load(const Path& path, AssetServer& server)
    {
        wrn("Loader for {} has not been provided", TypeName<A>::value());
        return nullptr;
    }
};

template <>
class AssetLoader<Mesh> final : public AssetLoaderBase
{
    Mesh* load(const Path& path, AssetServer& server);
};

template <>
class AssetLoader<Texture2D> final : public AssetLoaderBase
{
    Texture2D* load(const Path& path, AssetServer& server);
};

template <>
class AssetLoader<TextureCubeMap> final : public AssetLoaderBase
{
    TextureCubeMap* load(const Path& path, AssetServer& server);
};

template <>
class AssetLoader<Shader> final : public AssetLoaderBase
{
    Shader* load(const Path& path, AssetServer& server) const
    {
        err("Shaders are currently not supported as user assets");
        return nullptr;
    };
};
}
