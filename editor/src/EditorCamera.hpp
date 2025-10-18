#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace siren::editor
{

class EditorCamera
{
public:
    EditorCamera(const int width, const int height) : m_width(width), m_height(height){};

    /// @brief The type of camera
    enum class CameraType { PERSPECTIVE, ORTHOGRAPHIC } m_cameraType = CameraType::PERSPECTIVE;

    void onUpdate(float delta, bool isMouseHovered);

    glm::mat4 getViewMat() const;
    glm::mat4 getProjMat() const;
    glm::mat4 getProjViewMat() const;

    glm::vec3 getPosition() const;
    glm::vec3 getViewDirection() const;
    float getYaw() const;
    float getPitch() const;

    CameraType getCameraType() const;
    void setCameraType(CameraType type);
    float getWidth() const;
    void setWidth(float width);
    float getHeight() const;
    void setHeight(float height);
    float getNearPlane() const;
    void setNearPlane(float nearPlane);
    float getFarPlane() const;
    void setFarPlane(float farPlane);
    float getSensitivity() const;
    void setSensitivity(float sensitivity);
    float getNormalSpeed() const;
    void setNormalSpeed(float normalSpeed);
    float getFastSpeed() const;
    void setFastSpeed(float fastSpeed);
    float getFov() const;
    void setFov(float fov);

private:
    // ======== Configurable Parameters ========

    glm::vec3 m_position{ 0 };
    float m_width;
    float m_height;
    float m_nearPlane   = 0.00001;
    float m_farPlane    = 1000;
    float m_sensitivity = 3;
    float m_normalSpeed = 3;
    float m_fastSpeed   = 6;
    float m_fov         = 75;

    // ======= Non Configurable Parameters =======

    glm::vec3 m_viewDirection{ 0, 0, 1 };
    const float m_rotationSpeed = 0.0002;
    float m_yaw                 = 0;
    float m_pitch               = 0;

    // ============= Internal State =============

    enum class CameraState { NORMAL, FREE_LOOK } m_cameraState = CameraState::NORMAL;

    void updateNormal(float delta);
    void updateFreeLook(float delta);
};

} // namespace siren::editor
