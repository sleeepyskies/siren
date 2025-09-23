#pragma once

#include "GLFW/glfw3.h"
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <slog.hpp>
#include <vector>

class Window
{
public:
    explicit Window(int width = 800, int height = 800, const std::string& title = "Window");
    ~Window();

    bool valid() const;
    bool shouldClose() const;
    void setShouldClose(bool shouldClose) const;
    void registerCallback(const std::function<void(GLFWwindow*, int, int)>& callback);
    void swapBuffers() const;
    glm::ivec2 getSize() const;
    int getWidth() const;
    int getHeight() const;
    void clearColorBuffer() const;
    void clearDepthBuffer() const;
    void clearBuffers() const;
    void setBackgroundColor(glm::vec4 color);
    void setTitle(const std::string& title);
    void setVsyncEnabled(bool enabled);
    glm::dvec2 getCursorPos() const;
    void setCursorPos(glm::dvec2 position) const;
    void setMouseEnabled(bool enabled) const;
    // TODO: Create Application class, this done for now just for global input access
    static GLFWwindow* handle();

private:
    static GLFWwindow* m_window; // FIXME: make not static
    std::string m_title{};
    std::vector<std::function<void(GLFWwindow*, int, int)>> m_callbacks{};
    int m_width  = 0;
    int m_height = 0;
    glm::vec4 m_backgroundColor{ 0 };
    bool m_vSyncEnabled = false;

    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
};
