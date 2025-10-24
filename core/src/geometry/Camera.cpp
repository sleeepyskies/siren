#include "Camera.hpp"

#include "../input/InputModule.hpp"
#include "core/Application.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace siren::core
{

void Camera::setViewportWidth(const int width)
{
    m_viewportWidth = width;
}

void Camera::setViewportHeight(const int height)
{
    m_viewportHeight = height;
}

void Camera::onUpdate(const float delta)
{
    move(delta);
    freeLook(delta);
}

glm::mat4 Camera::viewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_direction, {0, 1, 0});
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

    if (core::Input::isKeyPressed(core::KeyCode::W)) {
        dir += glm::vec3(0, 0, -1);
    }
    if (core::Input::isKeyPressed(core::KeyCode::A)) {
        dir += glm::vec3(-1, 0, 0);
    }
    if (core::Input::isKeyPressed(core::KeyCode::S)) {
        dir += glm::vec3(0, 0, 1);
    }
    if (core::Input::isKeyPressed(core::KeyCode::D)) {
        dir += glm::vec3(1, 0, 0);
    }
    if (core::Input::isKeyPressed(core::KeyCode::SPACE)) {
        dir += glm::vec3(0, 1, 0);
    }
    if (core::Input::isKeyPressed(core::KeyCode::L_SHIFT)) {
        dir += glm::vec3(0, -1, 0);
    }

    if (glm::length(dir) == 0) {
        return;
    }

    dir = glm::normalize(dir);

    const float deltaSpeed = m_speed * delta;
    m_position += deltaSpeed * (dir.x * glm::normalize(glm::cross(m_direction, {0, 1, 0})));
    m_position += deltaSpeed * (dir.y * glm::vec3{0, 1, 0});
    m_position -= deltaSpeed * (dir.z * m_direction);
}

void Camera::freeLook(const float delta)
{
    const core::WindowsWindow& window = core::App::get().getWindow();

    // we use quaternion based rotation
    if (core::Input::isMouseKeyPressed(core::MouseCode::LEFT)) {
        if (m_isLooking) {
            // prevents mouse jump when first clicking
            core::Input::setMousePosition(glm::dvec2{m_viewportWidth, m_viewportHeight} / 2.);
            m_isLooking = false;
        }

        core::Input::setMouseMode(core::MouseMode::LOCKED);

        const glm::dvec2 mousePosition = core::Input::getMousePosition();
        const float deltaSensitivity   = m_sensitivity * m_rotationSpeed;
        const float deltaX             = mousePosition.x - (m_viewportWidth / 2);
        const float deltaY             = mousePosition.y - (m_viewportHeight / 2);

        const glm::quat pitch = glm::angleAxis(-deltaY * deltaSensitivity,
                                               glm::normalize(glm::cross(m_direction, {0, 1, 0})));
        const glm::quat yaw   = glm::angleAxis(-deltaX * deltaSensitivity, glm::vec3{0, 1, 0});
        const glm::quat orientation = yaw * pitch;

        m_direction = glm::normalize(orientation * m_direction);

        core::Input::setMousePosition(glm::dvec2{m_viewportWidth, m_viewportHeight} / 2.);
    } else {
        core::Input::setMouseMode(core::MouseMode::VISIBLE);
        m_isLooking = true;
    }
}

} // namespace siren::core
