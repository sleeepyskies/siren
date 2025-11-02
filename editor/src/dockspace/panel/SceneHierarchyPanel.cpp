#include "SceneHierarchyPanel.hpp"

#include "EditorContextComponent.hpp"

#include "ecs/Components.hpp"
#include "../../../../core/src/utilities/ImGui.hpp"


namespace siren::editor
{

void SceneHierarchyPanel::onUiRender()
{
    // Toolbar
    {
        if (ImGui::Button("New Entity")) {
            const auto entity = m_scene->create();
            m_scene->emplace<core::TagComponent>(entity, "Unnamed");
        }
    }

    const auto editorContext = m_scene->getSingletonSafe<EditorContextComponent>();
    if (!editorContext) { return; }

    for (const auto e : m_scene->getAll()) {
        const auto tag                = m_scene->getSafe<core::TagComponent>(e);
        const std::string displayName = tag ? tag->tag : "Unnamed Entity";
        if (ImGui::Selectable(displayName.c_str())) {
            editorContext->selectedEntity = e;
        }

        // double left click → rename
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            ImGui::OpenPopup("RenameEntity##");
        }

        // popup
        if (ImGui::BeginPopup("RenameEntity##")) {
            static char buffer[128];
            std::strncpy(buffer, displayName.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (tag) tag->tag = buffer;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) { }
    }
}

std::string SceneHierarchyPanel::getName() const
{
    return m_name;
}

} // namespace siren::editor
