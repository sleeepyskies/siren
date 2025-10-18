#pragma once

namespace siren::ui
{

class UiManager
{
public:
    UiManager()  = default;
    ~UiManager() = default;

    void init(bool multiViewport = false);
    void shutDown();

    void begin() const;
    void end() const;

private:
    bool m_multiViewportEnabled;
    bool m_setup = false;
};

} // namespace siren::ui
