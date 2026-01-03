#include "ShaderLibrary.hpp"

#include "assets/AssetModule.hpp"

// todo: these shaders shouldn't be managed by the AssetModule, but here

namespace siren::core
{
void ShaderLibrary::Import(const Path& path, const std::string& alias)
{
    const auto handle = Assets().importAsset(path);
    m_cache[alias]    = handle;
}

Ref<Shader> ShaderLibrary::Get(const std::string& alias)
{
    const auto handle = m_cache[alias];
    return Assets().GetAsset<Shader>(handle);
}
} // namespace siren::core
