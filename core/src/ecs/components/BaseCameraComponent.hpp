#pragma once

#include "ecs/core/Component.hpp"

namespace siren::ecs
{

/**
 * @brief A base class that all new CameraComponents should derive from. Doing so makes it usable by
 * the RenderSystem.
 */
struct BaseCameraComponent : Component {
    float viewportWidth, viewportHeight;
    float nearPlane, farPlane;

    glm::vec3 position{};

    virtual glm::mat4 getProjMat() const = 0;
    virtual glm::mat4 getViewMat() const = 0;
    virtual glm::mat4 getProjViewMat() const
    {
        return getProjMat() * getViewMat();
    }
};

} // namespace siren::ecs
