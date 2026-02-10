/**
 * @file Window.hpp
 */
#pragma once

#include "core/App.hpp"

#include "input/InputCodes.hpp"


namespace siren::core
{
/**
 * @brief The Window is an abstraction over the backends window.
 */
class Window {
public:
    /// @brief A struct containing general window data for window creation
    struct Properties {
        std::string title         = "Siren";
        bool vSyncEnabled         = true;
        u32 height                = 720;
        u32 width                 = 1280;
        glm::vec4 backgroundColor = glm::vec4(0, 0, 0, 1);
    };

    explicit Window(const Properties& properties) : m_properties(properties) { };

    Window() : m_properties(Properties()) { };

    virtual ~Window() = default;

    /// @brief Polls the window for events.
    virtual void poll_events() = 0;
    /// @brief Returns true if the window should be closed.
    virtual bool should_close() const = 0;
    /// @brief Presents the back buffer to the screen.
    virtual void swap_buffers() const = 0;
    /// @brief Returns the size of this window.
    virtual glm::ivec2 size() const = 0;
    /// @brief Sets the title of the window.
    virtual void set_title(const std::string& title) = 0;
    /// @brief Enables or disables vSync.
    virtual void set_vsync(bool value) = 0;
    /// @brief Returns the current mouse mode of this window.
    virtual MouseMode mouse_mode() const = 0;
    virtual void set_mouse_mode(MouseMode mode) = 0;
    /// @brief Returns the handle of the underlying window.
    virtual void* handle() = 0;

protected:
    Properties m_properties;

    /// @brief Sets up event emitters/posters
    virtual void setup_callbacks() const = 0;
};
} // namespace siren::core
