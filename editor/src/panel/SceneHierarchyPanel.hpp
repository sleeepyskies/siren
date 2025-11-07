#pragma once

#include "Panel.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{

class SceneHierarchyPanel final : public Panel
{
public:
    explicit SceneHierarchyPanel(const Ref<core::Scene>& scene) : m_scene(scene) { };

    void draw() override;

    std::string getName() const override { return "Scene Hierarchy"; }

private:
    Ref<core::Scene> m_scene = nullptr;

    void drawToolbar();
    void drawPanel();
};

} // namespace siren::editor
