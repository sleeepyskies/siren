#pragma once

#include "ecs/Components.hpp"
#include "ecs/core/Component.hpp"
#include "ecs/core/EntityManager.hpp"

namespace siren::ecs
{

/**
 * @brief A component holding all relevant information needed for the RenderSystem.
 */
struct RenderContextComponent final : Component {
    // HACK: maybe don't use a raw pointer here???
    BaseCameraComponent* cameraComponent = nullptr;

    explicit RenderContextComponent(BaseCameraComponent* camera) : cameraComponent(camera)
    {
    }
};

} // namespace siren::ecs
