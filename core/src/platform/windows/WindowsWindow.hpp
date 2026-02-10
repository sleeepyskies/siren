#pragma once

#include "platform/GL.hpp"
#include "window/Window.hpp"


namespace siren::platform
{
/**
 * @brief A glfw specific implementation of @ref Window
 */
class WindowsWindow final : public core::Window
{
public:
    explicit WindowsWindow(const Properties& properties);
    ~WindowsWindow() override;

    void poll_events() override;
    bool should_close() const override;
    void swap_buffers() const override;
    glm::ivec2 size() const override;
    void set_title(const std::string& title) override;
    void set_vsync(bool value) override;
    core::MouseMode mouse_mode() const override;
    void set_mouse_mode(core::MouseMode mode) override;
    void* handle() override;

private:
    GLFWwindow* m_window = nullptr;

    void setup_callbacks() const override;
};
} // namespace siren::platform
