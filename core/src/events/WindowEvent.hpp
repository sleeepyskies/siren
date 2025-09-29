#pragma once

#include "events/Event.hpp"

namespace core
{

class WindowCloseEvent final : public Event
{
public:
    EVENT_TYPE(EventType::WindowClose, EventCategory::Window);
    std::string toString() const override
    {
        return "WindowCloseEvent";
    }
};

class WindowResizeEvent final : public Event
{
public:
    WindowResizeEvent(const float w, const float h) : m_width(w), m_height(h)
    {
    }

    float getWidth() const
    {
        return m_width;
    }
    float getHeight() const
    {
        return m_height;
    }

    EVENT_TYPE(EventType::WindowResize, EventCategory::Window);
    std::string toString() const override
    {
        return "WindowResizeEvent";
    }

private:
    float m_width, m_height;
};

} // namespace core