#include "window.hpp"

#include <GLFW/glfw3.h>
#include "platform/opengl/debug.hpp"


namespace siren::core
{

Window::Window(const WindowSettings& descriptor) {
    Locator<>;

    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
}

Window::~Window() {
    TODO_IMPLEMENT_ME();
    entt::locator<>
}

} // namespace siren::core
