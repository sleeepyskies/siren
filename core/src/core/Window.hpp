#pragma once

#include "core/GL.hpp"
#include "events/Event.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

class Window
{
public:
    struct Properties {
        std::string title = "siren";

        uint32_t width    = 1280;
        uint32_t height   = 720;
        bool vSyncEnabled = true;
        glm::vec4 backgroundColor{ 0 };
    };

    using EventCallback = std::function<void(events::Event&)>;

    explicit Window(const Properties& properties);
    ~Window()                        = default;
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    void init();
    void destroy();

    void setEventCallback(const EventCallback& callback);

    bool shouldClose() const;
    void swapBuffers() const;
    glm::ivec2 getSize() const;
    void clearBuffers() const;
    void setTitle(const std::string& title);
    void setVsync(bool value);
    glm::dvec2 getCursorPos() const;
    void setCursorPos(glm::dvec2 position) const;
    void setMouseEnabled(bool enabled) const;
    GLFWwindow* handle() const;

private:
    GLFWwindow* m_window = nullptr;
    Properties m_properties;

    EventCallback m_eventCallback;

    void setCallbacks() const;
};

} // namespace siren::core
