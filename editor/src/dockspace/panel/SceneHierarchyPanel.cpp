#include "SceneHierarchyPanel.hpp"

#include "ui/ImGui.hpp"

namespace siren::editor
{

void SceneHierarchyPanel::onUiRender()
{
    for (const auto e : m_scene->getAll()) {
        ImGui::Text(std::format("{}", e).c_str()); //
    }
}

std::string SceneHierarchyPanel::getName() const
{
    return m_name;
}

} // namespace siren::editor