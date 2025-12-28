/**
 * @file WindowModule.hpp
 */
#pragma once

#include "core/Module.hpp"
#include "utilities/spch.hpp"
#include "window/Window.hpp"


namespace siren::core
{
/**
 * @brief The WindowModule manages the Window.
 * Currently, we only support one native Siren Window.
 */
class WindowModule final : public Module
{
public:
    bool initialize() override;
    void shutdown() override;

    const char* getName() override { return "WindowModule"; }

    /// @brief Polls the window for events.
    void pollEvents() const;
    /// @brief Returns true if the window should be closed.
    bool shouldClose() const;
    /// @brief Presents the back buffer to the screen.
    void swapBuffers() const;
    /// @brief Returns the size of this window.
    glm::ivec2 getSize() const;
    /// @brief Sets the title of the window.
    void setTitle(const std::string& title) const;
    /// @brief Enables or disables vSync.
    void setVsync(bool value) const;
    /// @brief Returns the mouse mode of the current window.
    MouseMode getMouseMode() const;
    /// @brief Sets the mouse mode of the current window.
    void setMouseMode(MouseMode mode) const;
    /// @brief Returns the handle of the underlying window.
    void* handle() const;

private:
    Own<Window> m_window = nullptr;
};
} // namespace siren::core
