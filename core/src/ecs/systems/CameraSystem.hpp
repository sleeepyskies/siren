#pragma once

#include "ecs/core/System.hpp"

#include "ecs/components/CameraComponent.hpp"
#include "ecs/components/TransformComponent.hpp"

namespace siren::ecs
{

class CameraSystem final : public ecs::System
{
public:
    void onUpdate(float delta, ecs::Scene& scene) override;
    void onReady(ecs::Scene& scene) override;
    void onShutdown(ecs::Scene& scene) override;

private:
    void look(float delta, TransformComponent& transformComponent,
              CameraComponent& cameraComponent) const;
};

} // namespace siren::ecs
