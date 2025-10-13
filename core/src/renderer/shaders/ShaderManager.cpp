#include "ShaderManager.hpp"

namespace siren::renderer
{

ShaderManager::ShaderManager(const Path& workingDirectory)
{
}

assets::AssetHandle ShaderManager::loadShader(const MaterialKey& materialKey)
{
    // already compiled, just return
    if (m_shaderCache.contains(materialKey)) { return m_shaderCache[materialKey]; }

    // need to load and compile
    switch (materialKey.shadingMode) {
        case ShadingMode::LIT: {
            NotImplemented;
        }
        case ShadingMode::UNLIT: {
            NotImplemented;
        }
        case ShadingMode::PBR: {
        }
        default: {
            SirenAssert(false, "Invalid MaterialKey");
        }
    }
}

} // namespace siren::renderer
