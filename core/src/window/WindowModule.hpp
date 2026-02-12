/**
 * @file WindowModule.hpp
 */
#pragma once

#include "core/core.hpp"

#include "utilities/spch.hpp"
#include "window/Window.hpp"


namespace siren::core
{
/**
 * @brief The WindowModule manages the Window.
 * Currently, we only support one native Siren Window.
 */
class WindowModule {
public:
    WindowModule();

    /// @brief Polls the window for events.
    void poll_events() const;
    /// @brief Returns true if the window should be closed.
    bool should_close() const;
    /// @brief Presents the back buffer to the screen.
    void swap_buffers() const;
    /// @brief Returns the size of this window.
    glm::ivec2 size() const;
    /// @brief Sets the title of the window.
    void set_title(const std::string& title) const;
    /// @brief Enables or disables vSync.
    void set_vsync(bool value) const;
    /// @brief Returns the mouse mode of the current window.
    MouseMode mouse_mode() const;
    /// @brief Sets the mouse mode of the current window.
    void set_mouse_mode(MouseMode mode) const;
    /// @brief Returns the handle of the underlying window.
    void* handle() const;

private:
    std::unique_ptr<Window> m_window = nullptr;
};
} // namespace siren::core
