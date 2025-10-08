#pragma once

#include <ecs/System.hpp>

#include "ecs/components/CameraComponent.hpp"
#include "ecs/components/TransformComponent.hpp"

namespace siren::ecs
{

class CameraSystem final : public secs::System
{
public:
    void onUpdate(float delta, secs::Scene& scene) override;
    void onReady(secs::Scene& scene) override;
    void onShutdown(secs::Scene& scene) override;

private:
    void look(const float delta, TransformComponent& transformComponent,
              CameraComponent& cameraComponent) const;
};

} // namespace siren::ecs
