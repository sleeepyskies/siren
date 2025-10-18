#pragma once

#include "core/Layer.hpp"
#include "dockspace/DockSpace.hpp"
#include "ecs/core/Scene.hpp"
#include "static-ui/StaticUi.hpp"

namespace siren::editor
{

class EditorLayer final : public core::Layer
{
public:
    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onUiRender() override;
    void onEvent(event::Event& e) override;
    std::string getName() const override;

private:
    Ref<ecs::Scene> m_scene = makeRef<ecs::Scene>();
    DockSpace m_dockSpace{ m_scene };
    StaticUi m_staticUi{};
};

} // namespace siren::editor
