#pragma once

#include "DockPanel.hpp"
#include "ecs/core/Scene.hpp"
#include "utilities/spch.hpp"

namespace siren::editor
{

class SceneHierarchyPanel final : public DockPanel
{
public:
    explicit SceneHierarchyPanel(const Ref<ecs::Scene>& scene) : m_scene(scene){};

    void onUiRender() override;
    std::string getName() const override;

private:
    Ref<ecs::Scene> m_scene = nullptr;
    std::string m_name      = "Hierarchy Panel";
};

} // namespace siren::editor
