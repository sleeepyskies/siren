#include "EditorCamera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "input/InputCodes.hpp"
#include "input/InputModule.hpp"


namespace siren::editor
{
bool EditorCamera::onUpdate(const float delta)
{
    const auto& inpt = core::input();

    // we only return true if we are performing a continuous action that steals mouse input
    const bool isRightHeld  = inpt.isMouseKeyHeld(core::MouseCode::RIGHT);
    const bool isMiddleHeld = inpt.isMouseKeyHeld(core::MouseCode::MIDDLE);

    if (isRightHeld && m_cameraState != CameraState::FREE_LOOK) {
        m_cameraState = CameraState::FREE_LOOK;
        inpt.setMouseMode(core::MouseMode::LOCKED);
    } else if (!isRightHeld && m_cameraState != CameraState::NORMAL) {
        m_cameraState = CameraState::NORMAL;
        // handle setting mouse mode in updateNormal()
    }

    switch (m_cameraState) {
        case CameraState::NORMAL: updateNormal(delta);
            break;
        case CameraState::FREE_LOOK: updateFreeLook(delta);
            break;
    }

    return isMiddleHeld || isRightHeld;
}

void EditorCamera::onResize(const int width, const int height)
{
    m_width  = width;
    m_height = height;
}

glm::mat4 EditorCamera::getViewMat() const
{
    return glm::lookAt(m_position, m_position + m_viewDirection, { 0, 1, 0 });
}

glm::mat4 EditorCamera::getProjMat() const
{
    const auto& [nearPlane, farPlane, sensitivity, speed, fov, type] = *m_properties;
    switch (type) {
            using enum Properties::CameraType;
        case PERSPECTIVE: {
            return glm::perspective(glm::radians(fov), m_width / m_height, nearPlane, farPlane);
        }
        case ORTHOGRAPHIC: {
            const float halfWidth  = m_width / 2;
            const float halfHeight = m_height / 2;
            return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
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

float EditorCamera::getWidth() const
{
    return m_width;
}

float EditorCamera::getHeight() const
{
    return m_height;
}

Ref<EditorCamera::Properties> EditorCamera::getProperties()
{
    return m_properties;
}

void EditorCamera::updateNormal(const float delta)
{
    const auto& inpt = core::input();

    // this state is always active, expect when pressing RMB
    if (inpt.isMouseKeyHeld(core::MouseCode::MIDDLE)) {
        // rotate around focal point
        inpt.setMouseMode(core::MouseMode::LOCKED);
        return;
    }

    // zoom on scroll
    inpt.setMouseMode(core::MouseMode::VISIBLE);
    const glm::vec2 scrollDelta = inpt.getScrollDelta();
    if (scrollDelta.y == 0) { return; } // no scroll, return

    // update cameras position along view dir
    const float scaledZoom = scrollDelta.y * m_zoomFactor;
    const float nextZoom   = m_currentZoom + scaledZoom;
    if (nextZoom <= m_zoomMax && nextZoom >= m_zoomMin) {
        m_currentZoom = nextZoom;
        m_position += m_viewDirection * scaledZoom;
    }
}

void EditorCamera::updateFreeLook(const float delta)
{
    const auto& inpt = core::input();
    // this state is only active when actively pressing the RMB.

    const auto& [nearPlane, farPlane, sensitivity, speed, fov, type] = *m_properties;

    // todo: update speed on scroll

    // handle looking
    {
        const glm::vec2 mouseDelta = inpt.getDeltaMousePosition();
        const float deltaSens      = sensitivity * m_rotationSpeed;

        m_yaw += mouseDelta.x * deltaSens;
        m_pitch += mouseDelta.y * deltaSens;
        m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

        m_viewDirection.x = cos(m_pitch) * sin(m_yaw);
        m_viewDirection.y = sin(m_pitch);
        m_viewDirection.z = cos(m_pitch) * cos(m_yaw);
        m_viewDirection   = glm::normalize(m_viewDirection);
    }

    // handle movement
    {
        glm::vec3 dir{ }; // use accumulative vector to avoid faster diagonal movement

        if (inpt.isKeyHeld(core::KeyCode::W)) { dir.z -= 1.0f; }
        if (inpt.isKeyHeld(core::KeyCode::S)) { dir.z += 1.0f; }
        if (inpt.isKeyHeld(core::KeyCode::A)) { dir.x += 1.0f; }
        if (inpt.isKeyHeld(core::KeyCode::D)) { dir.x -= 1.0f; }
        if (inpt.isKeyHeld(core::KeyCode::SPACE)) { dir.y += 1.0f; }
        if (inpt.isKeyHeld(core::KeyCode::L_CONTROL)) { dir.y -= 1.0f; }

        if (glm::length(dir) == 0) { return; } // no input, can skip all

        dir                     = glm::normalize(dir);
        const auto rotation     = glm::quat(glm::vec3(-m_pitch, m_yaw, 0));
        const glm::vec3 forward = rotation * glm::vec3(0, 0, -1);
        const glm::vec3 right   = rotation * glm::vec3(1, 0, 0);
        const glm::vec3 up      = glm::vec3(0, 1, 0);

        const glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

        const float finalSpeed = inpt.isKeyHeld(core::KeyCode::L_SHIFT) ? speed * 2 : speed;
        m_position += move * delta * finalSpeed;
    }
}
} // namespace siren::editor
