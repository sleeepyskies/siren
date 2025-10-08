#pragma once

#include "ecs/Component.hpp"

namespace siren::ecs
{
struct PlayerComponent final : secs::Component {
    float movementSpeed = 5.f;
};

} // namespace siren::ecs
