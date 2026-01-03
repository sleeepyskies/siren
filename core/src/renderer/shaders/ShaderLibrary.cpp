#include "ShaderLibrary.hpp"

#include "assets/AssetModule.hpp"

// todo: these shaders shouldn't be managed by the AssetModule, but here

namespace siren::core
{
void ShaderLibrary::import(const Path& path, const std::string& alias)
{
    const auto handle = assets().importAsset(path);
    m_cache[alias]    = handle;
}

Ref<Shader> ShaderLibrary::Get(const std::string& alias)
{
    const auto handle = m_cache[alias];
    return assets().GetAsset<Shader>(handle);
}
} // namespace siren::core
