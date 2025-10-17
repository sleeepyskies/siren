#pragma once

#include "core/GL.hpp"
#include "event/Event.hpp"
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

    explicit Window(const Properties& properties);
    ~Window()                        = default;
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    void init();
    void destroy();

    void setEventCallback(const event::EventCallback& callback);

    bool shouldClose() const;
    void close();

    void clearBuffers() const;
    void swapBuffers() const;

    glm::ivec2 getSize() const;

    void setTitle(const std::string& title);
    void setVsync(bool value);

    GLFWwindow* handle() const;

private:
    GLFWwindow* m_window = nullptr;
    Properties m_properties;

    event::EventCallback m_eventCallback = [](const event::Event& e) {};

    void setCallbacks() const;
};

} // namespace siren::core
