#pragma once

namespace siren::ui
{

class UIRenderer
{
public:
    UIRenderer()  = default;
    ~UIRenderer() = default;

    void init(bool multiViewport = false);
    void shutDown();

    void begin() const;
    void end() const;

private:
    bool m_multiViewportEnabled;
    bool m_setup = false;
};

} // namespace siren::ui
