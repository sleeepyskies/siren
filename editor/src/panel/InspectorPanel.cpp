#include "InspectorPanel.hpp"

#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "utilities/spch.hpp"
#include "assets/AssetModule.hpp"

// todo: this file could very much benefit from a reflection system, this is however not setup.
// once setup, alot of this boilerplate can be removed


namespace siren::editor
{

void InspectorPanel::draw()
{
    // todo: we really need a reflection system setup to remove all this boilerplate
    const auto selectedEntity = m_state->selectedEntity;

    if (!selectedEntity) {
        ImGuiSiren::Text("No entity selected.");
        return;
    }

    const auto& tag = m_state->scene.get<core::TagComponent>(selectedEntity);
    ImGuiSiren::Text("{}", tag.tag);

    drawComponents();
    drawAddComponents();
}

void InspectorPanel::drawComponents() const
{
    const auto& scene         = m_state->scene;
    const auto selectedEntity = m_state->selectedEntity;

    const auto mesh = scene.getSafe<core::MeshComponent>(selectedEntity);
    if (mesh) { drawMeshComponent(); }
    const auto transform = scene.getSafe<core::TransformComponent>(selectedEntity);
    if (transform) { drawTransformComponent(); }
}

void InspectorPanel::drawAddComponents() const
{
    if (ImGui::Button("Add Component +")) {
        ImGui::OpenPopup("AddComponentModal");
    }

    if (ImGui::BeginPopupModal("AddComponentModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGuiSiren::Text("Core Components");
        ImGui::ListBox(
            "##Core Components",
            &m_currentCoreComponent,
            m_coreComponents,
            IM_ARRAYSIZE(m_coreComponents),
            10
        );

        ImGuiSiren::Text("User Defined Components");

        if (ImGui::Button("Add")) {
            ImGui::CloseCurrentPopup();
            addComponentToEntity();
        }

        ImGui::EndPopup();
    }
}

void InspectorPanel::addComponentToEntity() const
{
    const char* componentString = m_coreComponents[m_currentCoreComponent];

    // todo: better solution that a bunch of if checks here lol
    if (std::strcmp("Mesh", componentString) == 0) {
        m_state->scene.emplace<core::MeshComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Script", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Transform", componentString) == 0) {
        m_state->scene.emplace<core::TransformComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Camera", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Directional Light", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Spot Light", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Point Light", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Skybox Light", componentString) == 0) {
        Todo;
    }
}

void InspectorPanel::drawMeshComponent() const
{
    ImGui::SeparatorText("Mesh");

    {
        // mesh selector
        if (ImGui::BeginCombo("Mesh", m_meshes[m_currentMesh])) {
            for (u32 i = 0; i < IM_ARRAYSIZE(m_meshes); i++) {
                const bool selected = m_currentMesh == i;
                if (ImGui::Selectable(m_meshes[i], selected)) {
                    m_currentMesh = i;
                    auto& mesh    = m_state->scene.get<core::MeshComponent>(m_state->selectedEntity);
                    core::PrimitiveParams params;
                    if (m_currentMesh == 0) { continue; }
                    if (m_currentMesh == 1) {
                        Todo;
                        continue;
                    }
                    if (m_currentMesh == 2) { params = core::CapsuleParams(); }
                    if (m_currentMesh == 3) { params = core::CubeParams(); }
                    if (m_currentMesh == 4) { params = core::PlaneParams(); }
                    mesh.meshHandle = core::assets().createPrimitive(params);
                }
                if (selected) { ImGui::SetItemDefaultFocus(); }
            }
            ImGui::EndCombo();
        }

        const auto& mesh = m_state->scene.get<core::MeshComponent>(m_state->selectedEntity);

        // mesh params editor
        if (m_currentMesh == 2) {
            // capsule
            if (mesh.meshHandle) {
                auto& capsuleParams = std::get<core::CapsuleParams>(
                    core::assets().getMetaData(mesh.meshHandle)->getPrimitiveParams()
                );
                auto& [radiusNew, heightNew, segmentsNew] = capsuleParams;
                auto [radiusOld, heightOld, segmentsOld]  = capsuleParams;
                ImGuiSiren::Text("Capsule Parameters");
                ImGui::SliderFloat("Radius", &radiusNew, 0, 1000);
                ImGui::SliderFloat("Height", &heightNew, 0, 1000);
                ImGuiSiren::SliderUInt("Segments", segmentsNew, 0, 1000);
                const bool changed = heightNew != heightOld || radiusNew != radiusOld || segmentsNew != segmentsOld;
                if (changed) {
                    core::assets().reloadAsset(mesh.meshHandle);
                }
            }
        }
        // material editor
    }
}

void InspectorPanel::drawTransformComponent() const
{
    auto& transform = m_state->scene.get<core::TransformComponent>(m_state->selectedEntity);

    ImGui::SeparatorText("Transform");

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 100);

    // Position
    ImGui::Text("Position");
    ImGui::NextColumn();
    ImGui::PushID("pos");
    ImGui::DragFloat3("##Position", &transform.translation.x, 0.1f);
    ImGui::PopID();
    ImGui::NextColumn();

    // Rotation
    ImGui::Text("Rotation");
    ImGui::NextColumn();
    ImGui::PushID("rot");
    ImGui::DragFloat3("##Rotation", &transform.rotation.x, 0.5f);
    ImGui::PopID();
    ImGui::NextColumn();

    // Scale
    ImGui::Text("Scale");
    ImGui::NextColumn();
    ImGui::PushID("scale");
    ImGui::DragFloat3("##Scale", &transform.scale.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();
    if (ImGui::Button("Reset##Scale")) {
        transform.scale = glm::vec3(1, 1, 1);
    }
    ImGui::PopID();
    ImGui::NextColumn();

    ImGui::Columns(1);
}

} // namespace siren::editor

