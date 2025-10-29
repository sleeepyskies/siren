#pragma once

#include "ecs/core/Component.hpp"

namespace siren::core
{
struct SpotLightComponent final : Component
{
    glm::vec3 position;
    glm::vec3 color;
    float innerCone;
    float outerCone;
};
} // namespace siren::ecs