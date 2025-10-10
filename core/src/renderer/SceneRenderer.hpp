#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/Scene.hpp"
#include "renderer/Shader.hpp"

namespace siren::renderer
{

class SceneRenderer
{
public:
    void setActiveCamera(ecs::EntityHandle cameraHandle);
    void setActiveShader(assets::AssetHandle shaderHandle);

    // HACK: should not pass shader here, need some ShaderManager thing
    void draw(ecs::Scene& scene) const;

private:
    ecs::EntityHandle m_activeCameraEntity   = utilities::UUID::invalid();
    assets::AssetHandle m_activeShaderHandle = utilities::UUID::invalid();
};

} // namespace siren::renderer
