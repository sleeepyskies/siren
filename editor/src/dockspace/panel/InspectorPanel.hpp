#pragma once
#include "DockPanel.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{


class InspectorPanel final : public DockPanel
{
public:
    explicit InspectorPanel(const Ref<core::Scene>& scene) : m_scene(scene) { }

    void onUiRender() override;

    std::string getName() const override { return "InspectorPanel"; }

private:
    Ref<core::Scene> m_scene = nullptr;
};

} // namespace siren::editor
