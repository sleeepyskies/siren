#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "glm/vec3.hpp"
#include <Input.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <slog.hpp>

class Camera
{
public:
    Camera(const int width, const int height) : m_viewportWidth(width), m_viewportHeight(height)
    {
    }
    ~Camera() = default;

    void onUpdate(float delta, const Window& window);

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

    float m_speed       = 30.f; // units per second
    float m_sensitivity = 4.f;

    bool m_isLooking = true;

    glm::vec3 m_position{ 0, 0, -3 };
    glm::vec3 m_direction = glm::normalize(-m_position); // (0,0,0) - pos

    void freeLook(float delta, const Window& window);
    void move(float delta);
};
