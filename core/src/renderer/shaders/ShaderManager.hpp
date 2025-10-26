#pragma once

#include "assets/Asset.hpp"
#include "renderer/material/MaterialKey.hpp"

namespace siren::core
{

/**
 * @brief Is responsible for compiling and caching shaders. It will inspect the material key
 * provided and generate and compile a shader based on the materials needs.
 */
class ShaderManager
{
public:
    ShaderManager()  = default;
    ~ShaderManager() = default;

    /// @brief Creates a Shader based on the provided materialKey
    Maybe<AssetHandle> loadShader(const MaterialKey& materialKey);

private:
    HashMap<MaterialKey, AssetHandle> m_shaderCache{};
};
} // namespace siren::core
