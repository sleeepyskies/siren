#pragma once

#include "ecs/components/PlayerComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/System.hpp"

namespace siren::ecs
{

class PlayerControllerSystem final : public ecs::System
{
public:
    void onUpdate(float delta, ecs::Scene& scene) override;

private:
    void move(float delta, TransformComponent& transformComponent,
              const PlayerComponent& playerComponent) const;
};

} // namespace siren::ecs
