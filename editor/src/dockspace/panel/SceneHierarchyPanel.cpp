#include "SceneHierarchyPanel.hpp"

#include "EditorContextComponent.hpp"

#include "ecs/Components.hpp"
#include "utilities/ImGui.hpp"

#include "utilities/IDGenerator.hpp"


namespace siren::editor
{

void SceneHierarchyPanel::onUiRender()
{
    static core::IDGenerator eidgen{ "Unnamed " };

    const auto editorContext = m_scene->getSingletonSafe<EditorContextComponent>();
    if (!editorContext) { return; }

    // Toolbar
    {
        if (ImGui::Button("New Entity")) {
            const auto entity             = m_scene->create();
            const std::string entityTitle = eidgen.getNextStr();
            m_scene->emplace<core::TagComponent>(entity, entityTitle);
            m_scene->emplace<core::TransformComponent>(entity);
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete Entity")) {
            if (editorContext->selectedEntity) {
                const auto tag = m_scene->getSafe<core::TagComponent>(editorContext->selectedEntity);
                m_scene->destroy(editorContext->selectedEntity);
                if (tag) {
                    eidgen.onDelete(tag->tag);
                }
            }
        }
    }

    for (const auto e : m_scene->getAll()) {
        const auto tag                = m_scene->getSafe<core::TagComponent>(e);
        const std::string displayName = tag ? tag->tag : "Unnamed Entity";
        if (ImGui::Selectable(displayName.c_str())) {
            editorContext->selectedEntity = e;
        }

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            ImGui::OpenPopup("RenameEntity##");
        }

        if (ImGui::BeginPopup("RenameEntity##")) {
            static char buffer[128];
            std::strncpy(buffer, displayName.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (tag) { tag->tag = buffer; }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) { }
    }
}

} // namespace siren::editor
