#pragma once

#include "ecs/core/Component.hpp"

namespace siren::ecs
{
struct DirectionalLightComponent final : Component {
    float angle;
    glm::vec3 color;
};

} // namespace siren::ecs
