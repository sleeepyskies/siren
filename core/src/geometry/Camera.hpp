#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "utilities/spch.hpp"

namespace siren::core
{

class Camera
{
public:
    Camera(const int width, const int height) : m_viewportWidth(width), m_viewportHeight(height) {}
    ~Camera() = default;

    void onUpdate(float delta);

    void setViewportWidth(int width);
    void setViewportHeight(int height);

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const;
    glm::mat4 projectionViewMatrix() const;
    glm::vec3 position() const;

private:
    float m_viewportWidth  = 800;
    float m_viewportHeight = 600;

    float m_fov  = 75.f;
    float m_near = 0.1f;
    float m_far  = 1000.f;

    float m_speed         = 5.f; // units per second
    float m_sensitivity   = 3.f;
    float m_rotationSpeed = 0.002f; // used so that sensitivity can be in a more reasonable range

    bool m_isLooking = true;

    glm::vec3 m_position{0, 0, -3};
    glm::vec3 m_direction = glm::normalize(-m_position); // (0,0,0) - pos

    void freeLook(float delta);
    void move(float delta);
};

} // namespace siren::core
