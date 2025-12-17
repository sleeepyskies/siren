#include "InspectorPanel.hpp"

#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "utilities/spch.hpp"
#include "assets/AssetModule.hpp"
#include "renderer/material/Material.hpp"


namespace siren::editor
{

void InspectorPanel::draw()
{
    const core::EntityHandle selectedEntity = m_state->selectedEntity;

    if (!selectedEntity) {
        ImGuiSiren::Text("No entity selected.");
        return;
    }

    const auto& tag = m_state->scene.get<core::TagComponent>(selectedEntity); // selected entity must have tag
    ImGuiSiren::Text("{}", tag.tag);

    drawComponents();
    drawAddComponents();
}

void InspectorPanel::drawComponents() const
{
    const auto& scene         = m_state->scene;
    const auto selectedEntity = m_state->selectedEntity;

    // todo: get reflection working for this
    /*
    const auto mesh = scene.getSafe<core::MeshComponent>(selectedEntity);
    if (mesh) { drawMeshComponent(); }
    const auto transform = scene.getSafe<core::TransformComponent>(selectedEntity);
    if (transform) { drawTransformComponent(); }
    */
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

} // namespace siren::editor

