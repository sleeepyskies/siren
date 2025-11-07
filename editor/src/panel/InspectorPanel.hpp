#pragma once
#include "Panel.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{


class InspectorPanel final : public Panel
{
public:
    explicit InspectorPanel(const Ref<core::Scene>& scene) : m_scene(scene) { }

    void draw() override;

    std::string getName() const override { return "Inspector Panel"; }

private:
    Ref<core::Scene> m_scene = nullptr;
};

} // namespace siren::editor
