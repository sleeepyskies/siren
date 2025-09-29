#pragma once

#include "core/GL.hpp"
#include "events/Event.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Window
{
public:
    struct Specification {
        std::string title = "siren";

        uint32_t width    = 1280;
        uint32_t height   = 720;
        bool vSyncEnabled = true;
        glm::vec4 backgroundColor{ 0 };
    };

    using EventCallback = std::function<void(Event&)>;

    explicit Window(const Specification& specification);
    ~Window() = default;

    void init();
    void destroy();

    void setEventCallback(const EventCallback& callback);

    bool shouldClose() const;
    void swapBuffers() const;
    glm::ivec2 getSize() const;
    void clearBuffers() const;
    void setTitle(const std::string& title);
    glm::dvec2 getCursorPos() const;
    void setCursorPos(glm::dvec2 position) const;
    void setMouseEnabled(bool enabled) const;
    GLFWwindow* handle() const;

private:
    EventCallback m_eventCallback;

    GLFWwindow* m_window = nullptr;
    Specification m_specification;

    void setCallbacks() const;
};

} // namespace core
