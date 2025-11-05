#pragma once

#include "DockPanel.hpp"
#include "ecs/core/Scene.hpp"
#include "utilities/spch.hpp"


namespace siren::editor
{

class SceneHierarchyPanel final : public DockPanel
{
public:
    explicit SceneHierarchyPanel(const Ref<core::Scene>& scene) : m_scene(scene) { };

    void onUiRender() override;

    std::string getName() const override { return "Scene Hierarchy"; }

private:
    Ref<core::Scene> m_scene = nullptr;
};

} // namespace siren::editor
