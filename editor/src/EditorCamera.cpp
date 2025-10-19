#include "EditorCamera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "core/Input.hpp"
#include "core/InputCodes.hpp"
#include "utilities/SirenAssert.hpp"

namespace siren::editor
{

bool EditorCamera::onUpdate(const float delta)
{
    const bool leftMousePressed = core::Input::isMouseKeyPressed(core::MouseCode::LEFT);

    if (leftMousePressed && m_cameraState != CameraState::FREE_LOOK) {
        dbg("Changing EditorCamera state to FREE_LOOK mousePressed");
        m_cameraState = CameraState::FREE_LOOK;
        core::Input::setMouseMode(core::MouseMode::LOCKED);
    } else if (!leftMousePressed && m_cameraState != CameraState::NORMAL) {
        dbg("Changing EditorCamera state to NORMAL !leftMousePressed");
        m_cameraState = CameraState::NORMAL;
        core::Input::setMouseMode(core::MouseMode::VISIBLE);
    }

    switch (m_cameraState) {
        case CameraState::NORMAL   : updateNormal(delta); break;
        case CameraState::FREE_LOOK: updateFreeLook(delta); break;
    }

    return leftMousePressed;
}

void EditorCamera::onResize(const int width, const int height)
{
    m_width  = width;
    m_height = height;
}

void EditorCamera::updateNormal(float delta)
{
    // nothing for now
}

void EditorCamera::updateFreeLook(const float delta)
{
    const auto& [nearPlane, farPlane, sensitivity, speed, fov, type] = *m_properties;

    // handle looking
    {
        const glm::vec2 mouseDelta = core::Input::getDeltaMousePosition();
        const float deltaSens      = sensitivity * m_rotationSpeed;

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

        if (core::Input::isKeyPressed(core::KeyCode::W)) { dir.z -= 1.0f; }
        if (core::Input::isKeyPressed(core::KeyCode::S)) { dir.z += 1.0f; }
        if (core::Input::isKeyPressed(core::KeyCode::A)) { dir.x += 1.0f; }
        if (core::Input::isKeyPressed(core::KeyCode::D)) { dir.x -= 1.0f; }

        if (glm::length(dir) == 0) { return; } // no input, can skip all

        dir                     = glm::normalize(dir);
        const auto rotation     = glm::quat(glm::vec3(-m_pitch, m_yaw, 0));
        const glm::vec3 forward = rotation * glm::vec3(0, 0, -1);
        const glm::vec3 right   = rotation * glm::vec3(1, 0, 0);
        const glm::vec3 up      = glm::vec3(0, 1, 0);

        const glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

        const float finalSpeed =
            core::Input::isKeyPressed(core::KeyCode::L_SHIFT) ? speed * 2 : speed;
        m_position += move * delta * finalSpeed;
    }
}

glm::mat4 EditorCamera::getViewMat() const
{
    return glm::lookAt(m_position, m_position + m_viewDirection, { 0, 1, 0 });
}

glm::mat4 EditorCamera::getProjMat() const
{
    const auto& [nearPlane, farPlane, sensitivity, speed, fov, type] = *m_properties;
    switch (type) {
        case CameraType::PERSPECTIVE: {
            return glm::perspective(glm::radians(fov), m_width / m_height, nearPlane, farPlane);
        }
        case CameraType::ORTHOGRAPHIC: {
            const float left   = -(m_width / 2);
            const float right  = m_width / 2;
            const float bottom = -(m_height / 2);
            const float top    = m_height / 2;
            return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
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

} // namespace siren::editor
