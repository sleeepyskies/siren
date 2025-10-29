#include "DockSpace.hpp"

#include "panel/SceneHierarchyPanel.hpp"
#include "panel/SceneViewPanel.hpp"


namespace siren::editor
{

DockSpace::DockSpace(const Ref<core::Scene>& scene) : m_scene(scene)
{
    // todo: read from some save file how we want to setup UI, but for now just hardcode
    m_panels.emplace_back(createOwn<SceneViewPanel>(m_scene));
    m_panels.emplace_back(createOwn<SceneHierarchyPanel>(m_scene));
}

void DockSpace::onUpdate(const float delta)
{
    for (const auto& panel : m_panels) { panel->onUpdate(delta); }
}

void DockSpace::onRender() const
{
    for (const auto& panel : m_panels) { panel->onRender(); }
}

void DockSpace::onUiRender() const
{
    ImGui::DockSpaceOverViewport();
    for (const auto& panel : m_panels) { panel->renderUi(); }
}

} // namespace siren::editor
