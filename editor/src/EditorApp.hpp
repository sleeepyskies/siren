#pragma once

#include "core/App.hpp"
#include "ecs/core/Scene.hpp"
#include "panel/Panel.hpp"
#include "widget/MainMenuBar.hpp"


namespace siren::editor
{

class EditorApp final : public core::App
{
public:
    void initialize() override;
    void onUpdate(float delta) override;
    void onRender() override;

    /// @brief Handles Editor setup.
    void setupEditor();

private:
    friend class App;
    ~EditorApp() override;

    explicit EditorApp(const Properties& properties)
        : App(properties) { }

    // todo: we dont want a hard limit one scene per EditorLayer.... SceneModule?
    Ref<core::Scene> m_scene = createRef<core::Scene>();
    std::vector<Own<Panel>> m_panels{ };
    MainMenuBar m_mainMenuBar{ };
};

} // namespace siren::editor
