#pragma once
#include <ecs/System.hpp>
#include <ecs/components/PlayerComponent.hpp>
#include <ecs/components/TransformComponent.hpp>

namespace siren::ecs
{

class PlayerControllerSystem final : public secs::System
{
public:
    void onUpdate(float delta, secs::Scene& scene) override;

private:
    void move(float delta, TransformComponent& transformComponent, const PlayerComponent& playerComponent) const;
};

} // namespace siren::ecs
