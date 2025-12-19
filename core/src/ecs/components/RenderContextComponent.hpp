#pragma once

#include "ecs/components/BaseCameraComponent.hpp"
#include "ecs/components/SkyLightComponent.hpp"
#include "ecs/core/Component.hpp"


namespace siren::core
{

/**
 * @brief A component holding all relevant information needed for the RenderSystem.
 */
struct RenderContextComponent final : Component
{
    // HACK: maybe don't use a raw pointer here???
    BaseCameraComponent* cameraComponent = nullptr;
    SkyLightComponent* skyBoxComponent   = nullptr;

    explicit RenderContextComponent(
        BaseCameraComponent* camera        = nullptr,
        SkyLightComponent* skyBoxComponent = nullptr
    ) : cameraComponent(camera), skyBoxComponent(skyBoxComponent) { }
};

} // namespace siren::ecs
