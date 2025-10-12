#pragma once

#include "ecs/components/BaseCameraComponent.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren::ecs
{
struct ThirdPersonCameraComponent final : BaseCameraComponent {
    glm::vec3 viewDirection = glm::vec3{ 0, 0, 1 };
    float fov               = 75;
    float yaw               = 0;
    float pitch             = 0;
    float sensitivity       = 3.f;
    float rotationSpeed     = 0.0005f;
    glm::vec3 focalOffset{ 0, 1, 0 }; // the focal offset of the models position
    float distanceOffset{ 2 };        // the distance the camera should be from the model

    glm::mat4 getViewMat() const override
    {
        return glm::lookAt(position, position + viewDirection, { 0, 1, 0 });
    }

    glm::mat4 getProjMat() const override
    {
        return glm::perspective(
            glm::radians(fov), viewportWidth / viewportHeight, nearPlane, farPlane);
    };
};

} // namespace siren::ecs
