#pragma once

#include "ecs/core/Component.hpp"

namespace siren::core
{

/**
 * @brief A base class that all new CameraComponents should derive from. Doing so makes it usable by
 * the RenderSystem.
 */
struct BaseCameraComponent : Component
{
    BaseCameraComponent(const int w, const int y) : viewportWidth(w), viewportHeight(y)
    {
    }

    float viewportWidth, viewportHeight;
    float nearPlane = 0.001;
    float farPlane  = 1000;

    glm::vec3 position{0};

    virtual glm::mat4 getProjMat() const = 0;
    virtual glm::mat4 getViewMat() const = 0;

    virtual glm::mat4 getProjViewMat() const
    {
        return getProjMat() * getViewMat();
    }
};

} // namespace siren::ecs
