#pragma once

#include "utilities/spch.hpp"

namespace core
{

struct DirectionalLight {
    glm::vec3 color{};
    glm::vec3 direction{};
};

struct PointLight {
    float cx, cy, cz; // color
    float _pad0;
    float px, py, pz; // pos
    float _pad1;
};

struct SpotLight {
    glm::vec3 color{};
    glm::vec3 position{};
    float innerCone = 0;
    float outerCone = 0;
};

} // namespace core
