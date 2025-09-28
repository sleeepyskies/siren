#pragma once

#include "GLFW/glfw3.h"
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <slog.hpp>
#include <vector>

namespace siren
{

class Window
{
public:
    struct Specification {
        std::string title;

        uint32_t width    = 1280;
        uint32_t height   = 720;
        bool vSyncEnabled = true;
        glm::vec4 backgroundColor{ 0 };
    };

    explicit Window(const Specification& specification = Specification());
    ~Window() = default;

    void init();
    void destroy();

    bool shouldClose() const;
    // TODO: replace this with emitting a Resize event
    void registerResizeCallback(const std::function<void(GLFWwindow*, int, int)>& callback);
    void swapBuffers() const;
    glm::ivec2 getSize() const;
    void clearBuffers() const;
    void setTitle(const std::string& title);
    glm::dvec2 getCursorPos() const;
    void setCursorPos(glm::dvec2 position) const;
    void setMouseEnabled(bool enabled) const;
    GLFWwindow* handle();

private:
    GLFWwindow* m_window = nullptr;
    Specification m_specification;

    std::vector<std::function<void(GLFWwindow*, int, int)>> m_callbacks{};

    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
};

} // namespace siren
