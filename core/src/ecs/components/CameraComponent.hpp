#pragma once

#include "ecs/Component.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren::ecs
{
struct CameraComponent final : secs::Component {
    float width, height;
    glm::vec3 position      = { 0, 0, -1 };
    glm::vec3 viewDirection = glm::vec3{ 0, 0, 1 };
    float near              = 0.1f;
    float far               = 1000.f;
    float fov               = 75;
    float sensitivity       = 3.f;
    float rotationSpeed     = 0.002f;
    glm::vec3 modelOffset{ -1 };

    float yaw   = 0.0f; // horizontal rotation
    float pitch = 0.0f; // vertical rotation

    CameraComponent(const int width, const int height) : width(width), height(height)
    {
    }
    CameraComponent() = delete;

    glm::mat4 viewMatrix() const
    {
        return glm::lookAt(position, position + viewDirection, { 0, 1, 0 });
    }

    glm::mat4 projectionMatrix() const
    {
        return glm::perspective(glm::radians(fov), width / height, near, far);
    };

    glm::mat4 projectionViewMatrix() const
    {
        return projectionMatrix() * viewMatrix();
    };
};

} // namespace siren::ecs
