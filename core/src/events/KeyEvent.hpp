#pragma once

#include "core/InputCodes.hpp"
#include "events/Event.hpp"

namespace siren::events
{

class KeyEvent : public Event
{
public:
    core::KeyCode getKeycode() const
    {
        return m_keycode;
    }

protected:
    explicit KeyEvent(const core::KeyCode keycode) : m_keycode(keycode)
    {
    }

    core::KeyCode m_keycode;
};

class KeyPressEvent final : public KeyEvent
{
public:
    explicit KeyPressEvent(const core::KeyCode keycode, const bool isRepeat = false)
        : KeyEvent(keycode), m_isRepeated(isRepeat)
    {
    }

    EVENT_TYPE(EventType::KeyPress, EventCategory::Key | EventCategory::Input, KeyPressEvent);

    std::string toString() const override
    {
        return std::format(
            "KeyPressEvent (key: {}, isRepeat: {})", static_cast<int>(m_keycode), m_isRepeated);
    }

private:
    bool m_isRepeated;
};

class KeyReleaseEvent final : public KeyEvent
{
public:
    explicit KeyReleaseEvent(const core::KeyCode keycode) : KeyEvent(keycode)
    {
    }

    EVENT_TYPE(EventType::MouseRelease, EventCategory::Key | EventCategory::Input, KeyReleaseEvent);

    std::string toString() const override
    {
        return std::format("KeyReleaseEvent (key: {})", static_cast<int>(m_keycode));
    }
};

} // namespace siren::events
