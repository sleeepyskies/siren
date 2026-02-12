#pragma once

#include "core/app.hpp"
#include "ecs/core/World.hpp"
#include "panel/Panel.hpp"
#include "widget/MainMenuBar.hpp"
#include "EditorState.hpp"


namespace siren::editor
{
class EditorApp final : public core::App
{
public:
    void init() override;
    void on_update(float delta) override;
    void on_render() override;

    /// @brief Handles Editor setup.
    void setupEditor();

private:
    explicit EditorApp(const Description& properties) : App(properties) { }

    ~EditorApp() override;
    friend class App;

    Own<EditorState> m_state = create_own<EditorState>();

    Vector<Own<Panel>> m_panels{ };
    MainMenuBar m_mainMenuBar{ };
};
} // namespace siren::editor
