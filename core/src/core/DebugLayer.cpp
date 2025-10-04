#include "DebugLayer.hpp"

#include "Application.hpp"

namespace core
{

void DebugLayer::onAttach()
{
    // pass
}

void DebugLayer::onDetach()
{
    // pass
}

void DebugLayer::onUpdate(const float delta)
{
    const float now = Application::get().getTime().elapsed();
    m_acuumulatedTime += (now - m_previousFrameElapsed);
    m_previousFrameElapsed = now;

    if (m_acuumulatedTime < 0.5) { return; }

    // update window with FPS only every 0.1s
    Window& window        = Application::get().getWindow();
    const float fps       = 1 / delta;
    const float frametime = delta * 1000;
    window.setTitle(std::format("siren FPS: {}, Frame time: {}", fps, frametime));
    m_acuumulatedTime = 0;
}

void DebugLayer::onRender()
{
    // pass
}

void DebugLayer::onEvent(Event& e)
{
    // pass
}

} // namespace core