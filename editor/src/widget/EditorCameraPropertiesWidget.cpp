#include "EditorCameraPropertiesWidget.hpp"

#include "ui/UI.hpp"
#include "ui/fonts/FontAwesome.hpp"

#include "utilities/ImGui.hpp"


namespace siren::editor
{

void EditorCameraPropertiesWidget::onRender()
{
    // todo make non static -> member variables PLEASE
    static const char* projectionItems[] = { "Perspective", "Orthographic" };
    static int current                   = m_cameraProperties->cameraType;
    // we want a 'floating button' effect

    bool fontPushed = true;
    ImGui::PushFont(UI::icon::Fas);
    if (ImGui::BeginMenu(FAS_GEAR)) {
        ImGui::PopFont();
        fontPushed = false;
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

        ImGui::EndMenu();
    }

    if (fontPushed) { ImGui::PopFont(); }
}

} // namespace siren::editor
