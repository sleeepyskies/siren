#include "ShaderManager.hpp"

#include <core/Application.hpp>

namespace siren::renderer
{

static HashMap<ShadingMode, Path> s_shadingModeToPath{
    { ShadingMode::LIT, "ass://shaders/basicLit.sshg" },
    { ShadingMode::UNLIT, "ass://shaders/basicUnlit.sshg" },
    { ShadingMode::PBR, "ass://shaders/basicPbr.sshg" },
};

Maybe<assets::AssetHandle> ShaderManager::loadShader(const MaterialKey& materialKey)
{
    // already compiled, just return
    if (m_shaderCache.contains(materialKey)) { return m_shaderCache[materialKey]; }

    // need to load and then cache
    const Path shaderPath = s_shadingModeToPath.at(materialKey.shadingMode);
    const auto res        = core::Application::get().getAssetManager().importAsset(shaderPath);
    if (!res) { return Nothing; }

    m_shaderCache[materialKey] = *res;
    return m_shaderCache[materialKey];
}

} // namespace siren::renderer
