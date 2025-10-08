#pragma once
#include <ecs/System.hpp>

namespace siren::ecs
{

class MovementSystem final : public secs::System
{
public:
    void onUpdate(float delta, secs::Scene& scene) override;
};

} // namespace siren::ecs
