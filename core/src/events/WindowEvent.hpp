#pragma once

#include "events/Event.hpp"

namespace core
{

class WindowCloseEvent final : public Event
{
public:
    EVENT_TYPE(EventType::WindowClose, EventCategory::Window, WindowCloseEvent);
    std::string toString() const override
    {
        return "WindowCloseEvent";
    }
};

class WindowResizeEvent final : public Event
{
public:
    WindowResizeEvent(const int w, const int h) : m_width(w), m_height(h)
    {
    }

    int getWidth() const
    {
        return m_width;
    }
    int getHeight() const
    {
        return m_height;
    }

    EVENT_TYPE(EventType::WindowResize, EventCategory::Window, WindowResizeEvent);
    std::string toString() const override
    {
        return std::format("WindowResizeEvent (width: {}, height: {})", m_width, m_height);
    }

    bool isHandled() const override
    {
        return false;
    }
    void handle() override
    {
        err("Attempting to consume a WindowResizeEvent. This should not happen");
    }

private:
    int m_width, m_height;
};

} // namespace core