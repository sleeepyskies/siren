#include "InspectorPanel.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"

#include "EditorContextComponent.hpp"


namespace siren::editor
{

void InspectorPanel::onRender()
{
    const auto editorContext = m_scene->getSingletonSafe<EditorContextComponent>();
    if (!editorContext) { return; }

    const core::EntityHandle entity = editorContext->selectedEntity;

    if (!entity) {
        ImGui::Text("No Entity Selected");
        return;
    }

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (ImGui::MenuItem("Transform")) {
            if (m_scene->hasComponent<core::TransformComponent>(entity)) {
                m_scene->emplace<core::TransformComponent>(entity);
            }
        }

        if (ImGui::MenuItem("Mesh")) {
            if (!m_scene->hasComponent<core::MeshComponent>(entity))
                m_scene->emplace<core::MeshComponent>(entity, core::AssetHandle::invalid());
        }

        ImGui::EndPopup();
    }

}

}

