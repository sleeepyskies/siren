#pragma once

namespace core
{

struct DirectionalLight {
    glm::vec3 color{};
    glm::vec3 direction{};
};

struct PointLight {
    glm::vec3 color{};
    glm::vec3 position{};
};

struct SpotLight {
    glm::vec3 color{};
    glm::vec3 position{};
    float innerCone = 0;
    float outerCone = 0;
};

} // namespace core
