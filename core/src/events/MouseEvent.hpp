#pragma once

#include "core/InputCodes.hpp"
#include "events/Event.hpp"

namespace core
{

class MouseEvent : public Event
{
public:
    MouseCode getMouseCode() const
    {
        return m_mouseKey;
    }

protected:
    explicit MouseEvent(const MouseCode button) : m_mouseKey(button)
    {
    }

    MouseCode m_mouseKey;
};

class MousePressEvent final : public MouseEvent
{
public:
    explicit MousePressEvent(const MouseCode mouseCode, const bool isRepeat = false)
        : MouseEvent(mouseCode), m_isRepeated(isRepeat)
    {
    }

    EVENT_TYPE(EventType::MousePress, EventCategory::Mouse | EventCategory::Input);

    std::string toString() const override
    {
        return std::format(
            "MousePressEvent (mouseKey: {}, isRepeat: {})", m_mouseKey, m_isRepeated);
    }

private:
    bool m_isRepeated;
};

class MouseReleaseEvent final : public MouseEvent
{
public:
    explicit MouseReleaseEvent(const MouseCode mouseCode) : MouseEvent(mouseCode)
    {
    }

    EVENT_TYPE(EventType::MouseRelease, EventCategory::Mouse | EventCategory::Input);

    std::string toString() const override
    {
        return std::format("MouseReleaseEvent (mouseKey: {})", m_mouseKey);
    }
};

} // namespace core