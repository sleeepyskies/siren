#pragma once

#include "event/Event.hpp"

namespace siren::event
{

class MouseScrollEvent final : public Event
{
public:
    MouseScrollEvent(const float x, const float y) : m_x(x), m_y(y){};

    EVENT_TYPE(EventType::MouseScroll, EventCategory::Mouse | EventCategory::Input,
               MouseScrollEvent);
    std::string toString() const override
    {
        return std::format("MouseScrollEvent (x: {}, y{})", m_x, m_y);
    }

    float getX() const
    {
        return m_x;
    }

    float getY() const
    {
        return m_y;
    }

private:
    float m_x, m_y;
};

} // namespace siren::event
