#pragma once
#include "Shader.hpp"


namespace siren::core
{
/**
 * @brief The ShaderLibrary is a cache used for core shaders only.
 */
class ShaderLibrary
{
public:
    /// @brief Imports and caches the core shader.
    void Import(const Path& path, const std::string& alias);
    /// @brief Returns the core shader.
    Ref<Shader> Get(const std::string& alias);

private:
    HashMap<std::string, AssetHandle> m_cache; //< Core Shader handles cached.
};
} // namespace siren::core
