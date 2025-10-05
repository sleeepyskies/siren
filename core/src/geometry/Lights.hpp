#pragma once

#include "utilities/spch.hpp"

namespace siren::geometry
{

struct DirectionalLight {
    glm::vec3 color{};
    glm::vec3 direction{};
};

struct PointLight {
    glm::vec3 color;
    float _pad0;
    glm::vec3 position;
    float _pad1;
};

struct SpotLight {
    glm::vec3 color{};
    glm::vec3 position{};
    float innerCone = 0;
    float outerCone = 0;
};

} // namespace siren::geometry
