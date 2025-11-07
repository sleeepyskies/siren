#pragma once

#include "Panel.hpp"
#include "ecs/core/Scene.hpp"

#include "utilities/IDGenerator.hpp"
#include "utilities/spch.hpp"


namespace siren::editor
{

class SceneHierarchyPanel final : public Panel
{
public:
    explicit SceneHierarchyPanel(const Ref<core::Scene>& scene) : m_scene(scene) { };

    void onRender() override;

    std::string getName() const override { return "Scene Hierarchy"; }

private:
    Ref<core::Scene> m_scene = nullptr;
    core::IDGenerator m_entityIDGenerator{ "Unnamed " };

    void drawToolbar();
    void drawPanel();
};

} // namespace siren::editor
