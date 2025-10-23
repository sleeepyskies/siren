/**
 * @file Window.hpp
 */
#pragma once

#include "core/App.hpp"

namespace siren::core
{

/**
 * @brief The Window is an abstraction over the backends window.
 */
class Window
{
public:
    /// @brief A struct containing general window data for window creation
    struct Properties {
        std::string title         = "siren window";
        bool vSyncEnabled         = true;
        u16 height                = 720;
        u16 width                 = 1280;
        glm::vec4 backgroundColor = glm::vec4(0, 0, 0, 1);
    };
    explicit Window(const Properties& properties) : m_properties(properties){};
    Window() : m_properties(Properties()){};

    virtual ~Window() = default;

    /// @brief Polls the window for events.
    virtual void pollEvents()                                               = 0;
    /// @brief Returns true if the window should be closed.
    virtual bool shouldClose() const                                        = 0;
    /// @brief Presents the back buffer to the screen.
    virtual void swapBuffers() const                                        = 0;
    /// @brief Returns the size of this window.
    virtual glm::ivec2 getSize() const                                      = 0;
    /// @brief Sets the title of the window.
    virtual void setTitle(const std::string& title)                         = 0;
    /// @brief Enables or disables vSync.
    virtual void setVsync(bool value)                                       = 0;
    /// @brief Returns the handle of the underlying window.
    virtual void* handle()                                                  = 0;
    /// @brief Used in order to use scroll information.
    virtual void setScrollCallback(std::function<void(glm::vec2)> callback) = 0;

protected:
    Properties m_properties;
};

} // namespace siren::core
