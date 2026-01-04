#include "WindowModule.hpp"

#include "core/App.hpp"
#include "platform/windows/WindowsWindow.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
bool WindowModule::Init()
{
    switch (app().getProperties().OS) {
        case App::Properties::OS::None: {
            err("Cannot init WindowModule with OS NONE");
            return false;
        }
        case App::Properties::OS::Windows: {
            m_window =
                    CreateOwn<platform::WindowsWindow>(Window::Properties()); // todo: load from disk
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

void WindowModule::pollEvents() const
{
    m_window->pollEvents();
}

bool WindowModule::shouldClose() const
{
    return m_window->shouldClose();
}

void WindowModule::swapBuffers() const
{
    return m_window->swapBuffers();
}

glm::ivec2 WindowModule::getSize() const
{
    return m_window->getSize();
}

void WindowModule::setTitle(const std::string& title) const
{
    return m_window->setTitle(title);
}

void WindowModule::setVsync(const bool value) const
{
    return m_window->setVsync(value);
}

MouseMode WindowModule::getMouseMode() const
{
    return m_window->getMouseMode();
}

void WindowModule::setMouseMode(const MouseMode mode) const
{
    return m_window->setMouseMode(mode);
}

void* WindowModule::handle() const
{
    return m_window->handle();
}
} // namespace siren::core
