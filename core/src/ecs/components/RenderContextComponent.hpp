#pragma once

#include "ecs/Components.hpp"
#include "ecs/core/Component.hpp"
#include "ecs/core/EntityManager.hpp"

namespace siren::ecs
{

/**
 * @brief A component holding all relevant information needed for the RenderSystem. Should be used
 * only as a singleton component.
 */
struct RenderContextComponent final : Component {
    // HACK: maybe don't use a raw pointer here???
    BaseCameraComponent* cameraComponent  = nullptr;
    // HACK: should not be hardcoded, update once ShaderManager exists
    assets::AssetHandle basicObjectShader = utilities::UUID::invalid();

    RenderContextComponent(BaseCameraComponent* camera, const assets::AssetHandle shader)
        : cameraComponent(camera), basicObjectShader(shader)
    {
    }
};

} // namespace siren::ecs
