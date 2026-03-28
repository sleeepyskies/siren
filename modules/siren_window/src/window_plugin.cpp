// ReSharper disable CppDeclarationHidesUncapturedLocal
module;

#include <GLFW/glfw3.h>

module siren.window;

import :systems;
import :signals;
import siren.log;

namespace siren::window {

auto WindowPlugin::construct(App& app) const -> void {
    app
           .add_resource<Window>(m_config)
           .add_resource<WindowEventState>(&*app.resource<Window>(), &*app.resource<ecs::EventBus>())
           .add_system(schedule::SchedulePhase::PreUpdate, poll_window_events, true);

    register_glfw_callbacks(app.resource<WindowEventState>());
}

auto WindowPlugin::shutdown(App& app) const -> void {
    app.remove_resources<Window, WindowEventState>();
}

auto WindowPlugin::register_glfw_callbacks(ecs::Resource<WindowEventState&> wes) const -> void {
    log::info("Registering glfw callbacks");

    glfwSetWindowUserPointer(wes->window->glfw_handle(), &*wes);

    glfwSetErrorCallback(
        [] (i32 error_code, const char* description) {
            log::error("GLFW Error encountered. Code: {}, description: {}", error_code, description);
        }
    );

    /*
    glfwSetMonitorCallback([] (GLFWmonitor* monitor, i32 event) { });
    glfwSetJoystickCallback([] (i32 jid, i32 event) { });
    */

    // glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* w, i32 width, i32 height) { });
    // glfwSetWindowContentScaleCallback(window, [] (GLFWwindow* w, f32 xscale, f32 yscale) { });
    // glfwSetWindowRefreshCallback(window, [] (GLFWwindow* w) { });
    // glfwSetWindowFocusCallback(window, [] (GLFWwindow* w, i32 focused) { });

    glfwSetWindowPosCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 x, const i32 y) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
            wes.window->m_position.set(glm::ivec2{ x, y });
            wes.event_bus->event_buffer<WindowMoveEvent>().push(WindowMoveEvent{ glm::uvec2{ x, y } });
        }
    );

    glfwSetWindowSizeCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 width, const i32 height) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
            wes.window->m_size.set(glm::uvec2{ width, height });
            wes.event_bus->event_buffer<WindowResizeEvent>().push(WindowResizeEvent{ glm::uvec2{ width, height } });
        }
    );

    glfwSetWindowCloseCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
            wes.window->close();
            wes.event_bus->event_buffer<WindowClosedEvent>().push(WindowClosedEvent{ });
        }
    );

    glfwSetWindowIconifyCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
                wes.window->m_window_mode.store(WindowMode::Minimized);
                wes.event_bus->event_buffer<WindowMinimizedEvent>().push(WindowMinimizedEvent{ });
            }
        }
    );

    glfwSetWindowMaximizeCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
                wes.window->m_window_mode.store(WindowMode::Maximized);
                wes.event_bus->event_buffer<WindowMaximizedEvent>().push(WindowMaximizedEvent{ });
            }
        }
    );

    // glfwSetDropCallback(window, [] (GLFWwindow* w, i32 count, const char** paths) { });
    // glfwSetCursorEnterCallback(window, [] (GLFWwindow* w, i32 entered) { });
    // glfwSetCharCallback(window, [] (GLFWwindow* w, u32 codepoint) { });

    glfwSetKeyCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 key, i32 scancode, const i32 action, const i32 mods) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));

            if (action == GLFW_PRESS) {
                wes.signal_bus->emit<GLFWKeyPressedSignal>(key, scancode, mods);
            } else if (action == GLFW_RELEASE) {
                wes.signal_bus->emit<GLFWKeyReleasedSignal>(key, scancode, mods);
            }
        }
    );

    glfwSetMouseButtonCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const i32 button, const i32 action, const i32 mods) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));

            if (action == GLFW_PRESS) {
                wes.signal_bus->emit<GLFWMouseButtonPressedSignal>(button, mods);
            } else if (action == GLFW_RELEASE) {
                wes.signal_bus->emit<GLFWMouseButtonReleasedSignal>(button, mods);
            }
        }
    );

    glfwSetCursorPosCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const double xpos, const double ypos) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
            wes.signal_bus->emit<GLFWMouseMotionSignal>(glm::vec2{ xpos, ypos });
        }
    );

    glfwSetScrollCallback(
        wes->window->glfw_handle(),
        [] (GLFWwindow* win, const double xoffset, const double yoffset) {
            const auto& wes = *static_cast<WindowEventState*>(glfwGetWindowUserPointer(win));
            wes.signal_bus->emit<GLFWMouseScrollSignal>(glm::vec2{ xoffset, yoffset });
        }
    );
}

} // namespace siren::window
