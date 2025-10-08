#pragma once

#include "ecs/Component.hpp"

namespace siren::ecs
{
struct CameraComponent final : secs::Component {
    glm::vec3 position;
    float width, height;
    float near, far;
    float fov;
    float speed;
    float sensitivity;
};

} // namespace siren::ecs
