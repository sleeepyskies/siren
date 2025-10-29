#pragma once

#include "core/Layer.hpp"
#include "dockspace/DockSpace.hpp"
#include "ecs/core/Scene.hpp"
#include "widget/MainMenuBar.hpp"


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

    const char* getName() const override { return "EditorLayer"; }

private:
    // todo: we dont want a hard limit one scene per EditorLayer....

    Ref<core::Scene> m_scene   = createRef<core::Scene>();
    Own<DockSpace> m_dockSpace = nullptr;
    MainMenuBar m_mainMenuBar{ };
};

} // namespace siren::editor
