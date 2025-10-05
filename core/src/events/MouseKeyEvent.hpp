#pragma once

#include "core/InputCodes.hpp"
#include "events/Event.hpp"

namespace siren::events
{

class MouseKeyEvent : public Event
{
public:
    core::MouseCode getMouseCode() const
    {
        return m_mouseKey;
    }

protected:
    explicit MouseKeyEvent(const core::MouseCode button) : m_mouseKey(button)
    {
    }

    core::MouseCode m_mouseKey;
};

class MousePressEvent final : public MouseKeyEvent
{
public:
    explicit MousePressEvent(const core::MouseCode mouseCode, const bool isRepeat = false)
        : MouseKeyEvent(mouseCode), m_isRepeated(isRepeat)
    {
    }

    EVENT_TYPE(EventType::MousePress, EventCategory::Mouse | EventCategory::Input, MousePressEvent);

    std::string toString() const override
    {
        return std::format(
            "MousePressEvent (mouseKey: {}, isRepeat: {})", (int) m_mouseKey, m_isRepeated);
    }

private:
    bool m_isRepeated;
};

class MouseReleaseEvent final : public MouseKeyEvent
{
public:
    explicit MouseReleaseEvent(const core::MouseCode mouseCode) : MouseKeyEvent(mouseCode)
    {
    }

    EVENT_TYPE(EventType::MouseRelease, EventCategory::Mouse | EventCategory::Input,
               MouseReleaseEvent);

    std::string toString() const override
    {
        return std::format("MouseReleaseEvent (mouseKey: {})", static_cast<int>(m_mouseKey));
    }
};

class MouseMoveEvent final : public Event
{
public:
    MouseMoveEvent(const float x, const float y)
    {
        m_x = x;
        m_y = y;
    }

    EVENT_TYPE(EventType::MouseMove, EventCategory::Mouse | EventCategory::Input, MouseMoveEvent);

    std::string toString() const override
    {
        return std::format("MouseMoveEvent (x: {}, y: {})", m_x, m_y);
    }

private:
    float m_x, m_y;
};

} // namespace siren::events