#include "WindowModule.hpp"

#include "core/App.hpp"
#include "platform/windows/WindowsWindow.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

bool WindowModule::initialize()
{
    switch (app().getProperties().OS) {
        case App::Properties::OS::NONE: {
            err("Cannot init WindowModule with OS NONE");
            return false;
        }
        case App::Properties::OS::WINDOWS: {
            m_window =
                createOwn<platform::WindowsWindow>(Window::Properties()); // todo: load from disk
            nfo("WindowModule initialised");
            return true;
        }
    }
    return false;
}
void WindowModule::shutdown()
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

void* WindowModule::handle() const
{
    return m_window->handle();
}

void WindowModule::setScrollCallback(std::function<void(glm::vec2)> callback)
{
    m_window->setScrollCallback(callback);
}

} // namespace siren::core