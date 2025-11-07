#pragma once

#include "EditorCamera.hpp"


namespace siren::editor
{

class EditorCameraPropertiesWidget
{
public:
    explicit EditorCameraPropertiesWidget(const Ref<EditorCamera::Properties>& cameraProperties)
        : m_cameraProperties(cameraProperties) { };

    void onRender();

private:
    Ref<EditorCamera::Properties> m_cameraProperties = nullptr;

    bool m_showCameraProperties = false;

    // todo: put into cameraProperties
    float m_nearPlaneMin   = 0.1f;
    float m_nearPlaneMax   = 10.f;
    float m_farPlaneMin    = 10.f;
    float m_farPlaneMax    = 100000.f;
    float m_sensitivityMin = 1.f;
    float m_sensitivityMax = 10.f;
    float m_speedMin       = 1.f;
    float m_speedMax       = 100.f;
    float m_fovMin         = 10.f;
    float m_fovMax         = 180.f;
};

} // namespace siren::editor
