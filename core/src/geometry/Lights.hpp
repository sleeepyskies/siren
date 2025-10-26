#pragma once

#include "utilities/spch.hpp"

namespace siren::core
{

struct DirectionalLight
{
    glm::vec3 direction{};
    glm::vec3 color{};
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
};

struct SpotLight
{
    glm::vec3 position{};
    glm::vec3 color{};
    float innerCone = 0;
    float outerCone = 0;
};

} // namespace siren::core
