#include "EditorCameraPropertiesWidget.hpp"

#include "utilities/ImGui.hpp"


namespace siren::editor
{

void EditorCameraPropertiesWidget::onRender()
{
    if (ImGui::Button("Camera Properties")) { ImGui::OpenPopup("CameraPropertiesPopup"); }

    static const char* projectionItems[] = { "Perspective", "Orthographic" };
    static int current                   = m_cameraProperties->cameraType;

    if (ImGui::BeginPopup("CameraPropertiesPopup")) {
        ImGui::SliderFloat(
            "Near Plane",
            &m_cameraProperties->nearPlane,
            m_nearPlaneMin,
            m_nearPlaneMax
        );
        ImGui::SliderFloat(
            "Far Plane",
            &m_cameraProperties->farPlane,
            m_farPlaneMin,
            m_farPlaneMax
        );
        ImGui::SliderFloat(
            "Sensitivity",
            &m_cameraProperties->sensitivity,
            m_sensitivityMin,
            m_sensitivityMax
        );
        ImGui::SliderFloat("Speed", &m_cameraProperties->speed, m_speedMin, m_speedMax);
        ImGui::SliderFloat("Fov", &m_cameraProperties->fov, m_fovMin, m_fovMax);
        if (ImGui::Combo("Camera Type", &current, projectionItems, IM_ARRAYSIZE(projectionItems))) {
            m_cameraProperties->cameraType = static_cast<EditorCamera::Properties::CameraType>(current);
        }
        ImGui::EndPopup();
    }
}

} // namespace siren::editor
