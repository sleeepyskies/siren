#pragma once

#include "core/App.hpp"
#include "ecs/core/Scene.hpp"
#include "panel/Panel.hpp"
#include "widget/MainMenuBar.hpp"
#include "EditorState.hpp"


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
    explicit EditorApp(const Properties& properties) : App(properties) { }

    ~EditorApp() override;
    friend class App;

    Own<EditorState> m_state = CreateOwn<EditorState>();

    Vector<Own<Panel>> m_panels{ };
    MainMenuBar m_mainMenuBar{ };
};
} // namespace siren::editor
