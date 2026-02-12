#include "WindowModule.hpp"

#include "core/app.hpp"
#include "core/locator.hpp"

#include "platform/windows/WindowsWindow.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
WindowModule::WindowModule() {
    switch (Locator<App>::locate().description().OS) {
        case App::Description::OS::None: {
            Logger::core->warn("Cannot init WindowModule with OS NONE");
        }
        case App::Description::OS::Windows: {
            m_window =
                    std::make_unique<platform::WindowsWindow>(Window::Properties()); // todo: load from disk
            Logger::core->info("WindowModule initialised");
        }
    }
}

void WindowModule::poll_events() const {
    m_window->poll_events();
}

bool WindowModule::should_close() const {
    return m_window->should_close();
}

void WindowModule::swap_buffers() const {
    return m_window->swap_buffers();
}

glm::ivec2 WindowModule::size() const {
    return m_window->size();
}

void WindowModule::set_title(const std::string& title) const {
    return m_window->set_title(title);
}

void WindowModule::set_vsync(const bool value) const {
    return m_window->set_vsync(value);
}

MouseMode WindowModule::mouse_mode() const {
    return m_window->mouse_mode();
}

void WindowModule::set_mouse_mode(const MouseMode mode) const {
    return m_window->set_mouse_mode(mode);
}

void* WindowModule::handle() const {
    return m_window->handle();
}
} // namespace siren::core
