#pragma once

#include "core/App.hpp"


namespace siren::editor
{

class EditorApp final : public core::App
{
public:
    static EditorApp& create(const Properties& properties);
    void init() override;

private:
    ~EditorApp() override = default;

    explicit EditorApp(const Properties& properties)
        : App(properties) { }
};

} // namespace siren::editor
