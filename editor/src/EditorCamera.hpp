#pragma once

#include "utilities/spch.hpp"
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>


namespace siren::editor
{

class EditorCamera
{
public:
    EditorCamera(const int width, const int height) : m_width(width), m_height(height) { }

    /// @brief Configurable properties.
    struct Properties
    {
        float nearPlane   = 0.1;
        float farPlane    = 1000;
        float sensitivity = 5;
        float speed       = 8;
        float fov         = 75;
        enum CameraType
        {
            PERSPECTIVE, ORTHOGRAPHIC
        } cameraType = PERSPECTIVE;
    };

    bool onUpdate(float delta);
    void onResize(int width, int height);

    glm::mat4 getViewMat() const;
    glm::mat4 getProjMat() const;
    glm::mat4 getProjViewMat() const;

    glm::vec3 getPosition() const;
    glm::vec3 getViewDirection() const;
    float getYaw() const;
    float getPitch() const;
    float getWidth() const;
    float getHeight() const;

    Ref<Properties> getProperties();

private:
    // ======= Non Configurable Parameters =======

    glm::vec3 m_position{ -3, 3, 3 };
    glm::vec3 m_viewDirection{ 3, -3, -3 };
    const float m_rotationSpeed = 0.0005;
    float m_yaw                 = 0;
    float m_pitch               = 0;
    float m_zoomFactor          = 0.2;
    float m_currentZoom         = 1;
    float m_zoomMin             = -10;
    float m_zoomMax             = 10;
    float m_width;
    float m_height;

    // ========= Configurable Parameters =========

    Ref<Properties> m_properties = create_ref<Properties>();

    // ============= Internal State =============

    enum class CameraState { NORMAL, FREE_LOOK } m_cameraState = CameraState::NORMAL;

    void updateNormal(float delta);
    void updateFreeLook(float delta);
};

} // namespace siren::editor
