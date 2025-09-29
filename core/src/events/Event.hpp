#pragma once

#include "utilities/spch.hpp"

namespace core
{

enum class EventType {
    None = 0,
    // Window Events

    WindowClose,
    WindowResize,
    // Key Events

    KeyPress,
    KeyRelease,
    // Mouse Events

    MousePress,
    MouseRelease,
    MouseMove
};

enum class EventCategory {
    None   = 0,
    Window = 0x1,
    Input  = 0x2,
    Key    = 0x4,
    Mouse  = 0x8,
};

#define EVENT_TYPE(type, categories)                                                               \
    EventType getType() const override                                                             \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static EventType staticType()                                                                  \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    int getCategoryFlags() const override                                                          \
    {                                                                                              \
        return static_cast<int>(categories);                                                       \
    }

class Event
{
public:
    virtual ~Event()                     = default;
    virtual EventType getType() const    = 0;
    virtual int getCategoryFlags() const = 0;
    virtual std::string toString() const = 0;
    // static EventType getType() const    = 0;

    bool isInCategory(EventCategory category) const
    {
        return getCategoryFlags() & static_cast<int>(category);
    }
    bool isHandled() const
    {
        return m_handled;
    }
    void handle()
    {
        m_handled = true;
    }

private:
    bool m_handled = false;
};

// TODO: not instantiate per event and make a queue of events maybe?
class EventDispatcher
{
public:
    template <typename E>
        requires(std::is_base_of_v<Event, E>)
    using EventFunction = std::function<bool(E&)>;

    explicit EventDispatcher(Event& event) : m_event(event)
    {
    }

    template <typename E> bool dispatch(EventFunction<E> eventFunction)
    {
        if (m_event.getType() == E::staticType() && !m_event.isHandled()) {
            if (eventFunction(m_event)) m_event.handle();
            return true;
        }
        return false;
    }

private:
    Event& m_event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& event)
{
    return os << event.toString();
}

inline int operator|(EventCategory lhs, EventCategory rhs)
{
    return static_cast<int>(lhs) | static_cast<int>(rhs);
}

} // namespace core
