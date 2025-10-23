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

    void pollEvents() override;
    bool shouldClose() const override;
    void swapBuffers() const override;
    glm::ivec2 getSize() const override;
    void setTitle(const std::string& title) override;
    void setVsync(bool value) override;
    void* handle() override;
    void setScrollCallback(std::function<void(glm::vec2)> callback) override;

private:
    GLFWwindow* m_window = nullptr;
    std::function<void(glm::vec2)> m_scrollCallback{};

    void setupCallbacks();
    static void glfwScrollCallback(GLFWwindow* win, double x, double y);
};

} // namespace siren::platform
