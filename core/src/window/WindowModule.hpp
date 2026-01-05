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
    bool Init() override;
    void Shutdown() override;

    const char* GetName() override { return "WindowModule"; }

    /// @brief Polls the window for events.
    void PollEvents() const;
    /// @brief Returns true if the window should be closed.
    bool ShouldClose() const;
    /// @brief Presents the back buffer to the screen.
    void SwapBuffers() const;
    /// @brief Returns the size of this window.
    glm::ivec2 GetSize() const;
    /// @brief Sets the title of the window.
    void SetTitle(const std::string& title) const;
    /// @brief Enables or disables vSync.
    void SetVsync(bool value) const;
    /// @brief Returns the mouse mode of the current window.
    MouseMode GetMouseMode() const;
    /// @brief Sets the mouse mode of the current window.
    void SetMouseMode(MouseMode mode) const;
    /// @brief Returns the handle of the underlying window.
    void* GetHandle() const;

private:
    Own<Window> m_window = nullptr;
};
} // namespace siren::core
