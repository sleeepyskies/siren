#include "Camera.hpp"

#include "core/Input.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace core
{

void Camera::setViewportWidth(const int width)
{
    m_viewportWidth = width;
}

void Camera::setViewportHeight(const int height)
{
    m_viewportHeight = height;
}

void Camera::onUpdate(const float delta, const Window& window)
{
    move(delta);
    freeLook(delta, window);
}

glm::mat4 Camera::viewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_direction, { 0, 1, 0 });
}

glm::mat4 Camera::projectionMatrix() const
{
    return glm::perspective(glm::radians(m_fov), m_viewportWidth / m_viewportHeight, m_near, m_far);
}

glm::mat4 Camera::projectionViewMatrix() const
{
    return projectionMatrix() * viewMatrix();
}

glm::vec3 Camera::position() const
{
    return m_position;
}

void Camera::move(const float delta)
{
    glm::vec3 dir{}; // use accumulative vector to avoid faster diagonal movement

    if (Input::isKeyPressed(W)) dir += glm::vec3(0, 0, -1);
    if (Input::isKeyPressed(A)) dir += glm::vec3(-1, 0, 0);
    if (Input::isKeyPressed(S)) dir += glm::vec3(0, 0, 1);
    if (Input::isKeyPressed(D)) dir += glm::vec3(1, 0, 0);
    if (Input::isKeyPressed(SPACE)) dir += glm::vec3(0, 1, 0);
    if (Input::isKeyPressed(L_SHIFT)) dir += glm::vec3(0, -1, 0);

    if (glm::length(dir) == 0) return;

    dir = glm::normalize(dir);

    const float deltaSpeed = m_speed * delta;
    m_position += deltaSpeed * (dir.x * glm::normalize(glm::cross(m_direction, { 0, 1, 0 })));
    m_position += deltaSpeed * (dir.y * glm::vec3{ 0, 1, 0 });
    m_position -= deltaSpeed * (dir.z * m_direction);
}

void Camera::freeLook(const float delta, const Window& window)
{
    // we use quaternion based rotation

    if (Input::isMouseKeyPressed(LEFT)) {
        if (m_isLooking) {
            // prevents mouse jump when first clicking
            window.setCursorPos(glm::dvec2{ m_viewportWidth, m_viewportHeight } / 2.);
            m_isLooking = false;
        }

        window.setMouseEnabled(false);

        const glm::dvec2 mousePosition = Input::getMousePosition();
        const float deltaX             = mousePosition.x - (m_viewportWidth / 2);
        const float deltaY             = mousePosition.y - (m_viewportHeight / 2);
        const float deltaSensitivity   = m_sensitivity * delta / 10.f;

        const glm::quat pitch = glm::angleAxis(
            -deltaY * deltaSensitivity, glm::normalize(glm::cross(m_direction, { 0, 1, 0 })));
        const glm::quat yaw = glm::angleAxis(-deltaX * deltaSensitivity, glm::vec3{ 0, 1, 0 });
        const glm::quat orientation = yaw * pitch;

        m_direction = glm::normalize(orientation * m_direction);

        window.setCursorPos(glm::dvec2{ m_viewportWidth, m_viewportHeight } / 2.);
    } else {
        window.setMouseEnabled(true);
        m_isLooking = true;
    }
}

} // namespace core
