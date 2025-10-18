#include "EditorCamera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "core/Input.hpp"
#include "core/InputCodes.hpp"
#include "utilities/SirenAssert.hpp"

namespace siren::editor
{

void EditorCamera::onUpdate(const float delta, const bool isMouseHovered)
{
    if (!isMouseHovered) {
        if (m_cameraState != CameraState::NORMAL) {
            m_cameraState = CameraState::NORMAL;
            core::Input::setMouseMode(core::MouseMode::VISIBLE);
        }
        return;
    }

    const bool leftMousePressed = core::Input::isMouseKeyPressed(core::MouseCode::LEFT);

    if (leftMousePressed && m_cameraState != CameraState::FREE_LOOK) {
        m_cameraState = CameraState::FREE_LOOK;
        core::Input::setMouseMode(core::MouseMode::LOCKED);
    } else if (!leftMousePressed && m_cameraState != CameraState::NORMAL) {
        m_cameraState = CameraState::NORMAL;
        core::Input::setMouseMode(core::MouseMode::VISIBLE);
    }

    switch (m_cameraState) {
        case CameraState::NORMAL   : return updateNormal(delta);
        case CameraState::FREE_LOOK: return updateFreeLook(delta);
    }
}

void EditorCamera::updateNormal(float delta)
{
    // nothing for now
}

void EditorCamera::updateFreeLook(float delta)
{
    // handle looking
    {
        const glm::vec2 mouseDelta = core::Input::getDeltaMousePosition();
        const float deltaSens      = m_sensitivity * m_rotationSpeed;

        m_yaw -= mouseDelta.x * deltaSens;
        m_pitch -= mouseDelta.y * deltaSens;
        m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

        m_viewDirection.x = cos(m_pitch) * sin(m_yaw);
        m_viewDirection.y = sin(m_pitch);
        m_viewDirection.z = cos(m_pitch) * cos(m_yaw);
        m_viewDirection   = glm::normalize(m_viewDirection);
    }

    // handle movement
    {
        glm::vec3 dir{}; // use accumulative vector to avoid faster diagonal movement

        if (core::Input::isKeyPressed(core::KeyCode::W)) { dir -= glm::vec3(0, 0, 1); }
        if (core::Input::isKeyPressed(core::KeyCode::A)) { dir -= glm::vec3(-1, 0, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::S)) { dir -= glm::vec3(0, 0, -1); }
        if (core::Input::isKeyPressed(core::KeyCode::D)) { dir -= glm::vec3(1, 0, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::SPACE)) { dir += glm::vec3(0, 1, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::L_SHIFT)) { dir += glm::vec3(0, -1, 0); }

        if (glm::length(dir) == 0) { return; } // no input, can skip all

        dir                     = glm::normalize(dir);
        const auto rotation     = glm::quat(glm::vec3(m_pitch, m_yaw, 0));
        const glm::vec3 forward = rotation * glm::vec3(0, 0, -1);
        const glm::vec3 right   = rotation * glm::vec3(1, 0, 0);
        const glm::vec3 up      = glm::vec3(0, 1, 0);

        glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

        const float speed =
            core::Input::isKeyPressed(core::KeyCode::L_SHIFT) ? m_fastSpeed : m_normalSpeed;
        m_position += move * delta * speed;
    }
}

glm::mat4 EditorCamera::getViewMat() const
{
    return glm::lookAt(m_position, m_position + m_viewDirection, { 0, 1, 0 });
}

glm::mat4 EditorCamera::getProjMat() const
{
    switch (m_cameraType) {
        case CameraType::PERSPECTIVE: {
            return glm::perspective(
                glm::radians(m_fov), m_width / m_height, m_nearPlane, m_farPlane);
        }
        case CameraType::ORTHOGRAPHIC: {
            const float left   = -(m_width / 2);
            const float right  = m_width / 2;
            const float bottom = -(m_height / 2);
            const float top    = m_height / 2;
            return glm::ortho(left, right, bottom, top, m_nearPlane, m_farPlane);
        }
    }

    SirenAssert(false, "Invalid EditorCamera State");
}

glm::mat4 EditorCamera::getProjViewMat() const
{
    return getProjMat() * getViewMat();
}

glm::vec3 EditorCamera::getPosition() const
{
    return m_position;
}

glm::vec3 EditorCamera::getViewDirection() const
{
    return m_viewDirection;
}

float EditorCamera::getYaw() const
{
    return m_yaw;
}

float EditorCamera::getPitch() const
{
    return m_pitch;
}

EditorCamera::CameraType EditorCamera::getCameraType() const
{
    return m_cameraType;
}

void EditorCamera::setCameraType(const CameraType type)
{
    m_cameraType = type;
}

float EditorCamera::getWidth() const
{
    return m_width;
}

void EditorCamera::setWidth(const float width)
{
    m_width = width;
}

float EditorCamera::getHeight() const
{
    return m_height;
}

void EditorCamera::setHeight(const float height)
{
    m_height = height;
}

float EditorCamera::getNearPlane() const
{
    return m_nearPlane;
}

void EditorCamera::setNearPlane(const float nearPlane)
{
    m_nearPlane = nearPlane;
}

float EditorCamera::getFarPlane() const
{
    return m_farPlane;
}

void EditorCamera::setFarPlane(const float farPlane)
{
    m_farPlane = farPlane;
}

float EditorCamera::getSensitivity() const
{
    return m_sensitivity;
}

void EditorCamera::setSensitivity(const float sensitivity)
{
    m_sensitivity = sensitivity;
}

float EditorCamera::getNormalSpeed() const
{
    return m_normalSpeed;
}

void EditorCamera::setNormalSpeed(const float normalSpeed)
{
    m_normalSpeed = normalSpeed;
}

float EditorCamera::getFastSpeed() const
{
    return m_fastSpeed;
}

void EditorCamera::setFastSpeed(const float fastSpeed)
{
    m_fastSpeed = fastSpeed;
}

float EditorCamera::getFov() const
{
    return m_fov;
}

void EditorCamera::setFov(const float fov)
{
    m_fov = fov;
}

} // namespace siren::editor
