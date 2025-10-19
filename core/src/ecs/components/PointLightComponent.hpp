#pragma once

#include "ecs/core/Component.hpp"

namespace siren::ecs
{
struct PointLightComponent final : Component {
    glm::vec3 position;
    glm::vec3 color;

    PointLightComponent(const glm::vec3& position, const glm::vec3& color)
        : position(position), color(color){};
};

} // namespace siren::ecs
