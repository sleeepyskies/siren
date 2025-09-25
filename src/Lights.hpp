#pragma once
#include "glm/vec3.hpp"

struct Light {
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
};

struct DirectionalLight : Light {
    glm::vec3 direction{};
};

struct PointLight : Light {
    glm::vec3 position{};
};

struct SpotLight : Light {
    glm::vec3 position{};
    float innerCone = 0;
    float outerCone = 0;
};
