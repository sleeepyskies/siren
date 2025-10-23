#pragma once

namespace siren::ui
{

// todo: i dont think we need a UI manager, its kinda overkill since we use ImGui. Maybe refactor

class UiManager
{
public:
    UiManager()  = default;
    ~UiManager() = default;

    void init();
    void shutDown();

    void begin() const;
    void end() const;

private:
    bool m_setup = false;
};

} // namespace siren::ui
