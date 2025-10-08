#pragma once

#include "ecs/core/Component.hpp"

namespace siren::ecs
{
struct PlayerComponent final : Component {
    float movementSpeed = 5.f;
};

} // namespace siren::ecs
