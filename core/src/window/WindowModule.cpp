#include "WindowModule.hpp"

#include "core/App.hpp"
#include "platform/windows/WindowsWindow.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
bool WindowModule::Init()
{
    switch (app().GetProperties().OS) {
        case App::Properties::OS::None: {
            err("Cannot init WindowModule with OS NONE");
            return false;
        }
        case App::Properties::OS::Windows: {
            m_window =
                    create_own<platform::WindowsWindow>(Window::Properties()); // todo: load from disk
            nfo("WindowModule initialised");
            return true;
        }
    }
    return false;
}

void WindowModule::Shutdown()
{
    m_window = nullptr;
}

void WindowModule::PollEvents() const
{
    m_window->pollEvents();
}

bool WindowModule::ShouldClose() const
{
    return m_window->shouldClose();
}

void WindowModule::SwapBuffers() const
{
    return m_window->swapBuffers();
}

glm::ivec2 WindowModule::GetSize() const
{
    return m_window->getSize();
}

void WindowModule::SetTitle(const std::string& title) const
{
    return m_window->setTitle(title);
}

void WindowModule::SetVsync(const bool value) const
{
    return m_window->setVsync(value);
}

MouseMode WindowModule::GetMouseMode() const
{
    return m_window->getMouseMode();
}

void WindowModule::SetMouseMode(const MouseMode mode) const
{
    return m_window->setMouseMode(mode);
}

void* WindowModule::GetHandle() const
{
    return m_window->handle();
}
} // namespace siren::core
