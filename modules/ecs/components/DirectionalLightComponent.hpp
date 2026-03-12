#pragma once

#include "ecs/core/Component.hpp"


namespace siren::core
{

struct DirectionalLightComponent final : Component
{
    glm::vec3 direction;
    glm::vec3 color;
};

} // namespace siren::ecs
