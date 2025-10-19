#include "SceneHierarchyPanel.hpp"

#include "ecs/Components.hpp"
#include "ui/ImGui.hpp"

namespace siren::editor
{

void SceneHierarchyPanel::onUiRender()
{
    for (const auto e : m_scene->getAll()) {
        const auto tag = m_scene->getSafe<ecs::TagComponent>(e);
        if (tag) {
            ImGui::Text(tag->tag.c_str());
        } else {
            ImGui::Text("Unnamed Entity");
        }
    }
}

std::string SceneHierarchyPanel::getName() const
{
    return m_name;
}

} // namespace siren::editor