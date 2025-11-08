#include "InspectorPanel.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{

void InspectorPanel::draw()
{
    const core::EntityHandle entity = m_state->selectedEntity;
    auto& scene                     = m_state->scene;

    if (!entity) {
        ImGui::Text("No Entity Selected");
        return;
    }

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (ImGui::MenuItem("Transform")) {
            if (scene.hasComponent<core::TransformComponent>(entity)) {
                scene.emplace<core::TransformComponent>(entity);
            }
        }

        if (ImGui::MenuItem("Mesh")) {
            if (!scene.hasComponent<core::MeshComponent>(entity))
                scene.emplace<core::MeshComponent>(entity, core::AssetHandle::invalid());
        }

        ImGui::EndPopup();
    }
}

} // namespace siren::editor

