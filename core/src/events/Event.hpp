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

#define EVENT_TYPE(type, categories, clazz)                                                        \
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
    }                                                                                              \
    Uref<Event> createUref() const override                                                        \
    {                                                                                              \
        return makeUref<clazz>(*this);                                                             \
    }

class Event
{
public:
    virtual ~Event()                       = default;
    virtual EventType getType() const      = 0;
    virtual int getCategoryFlags() const   = 0;
    virtual std::string toString() const   = 0;
    virtual Uref<Event> createUref() const = 0;
    // static EventType getType() const   = 0;

    bool isInCategory(EventCategory category) const
    {
        return getCategoryFlags() & static_cast<int>(category);
    }
    virtual bool isHandled() const
    {
        return m_handled;
    }
    virtual void handle()
    {
        m_handled = true;
    }

private:
    bool m_handled = false;
};

// TODO: not instantiate per event and make a queue of events maybe?
class EventHandler
{
public:
    template <typename E>
        requires(std::is_base_of_v<Event, E>)
    using EventFunction = std::function<bool(E&)>;

    explicit EventHandler(Event& event) : m_event(event)
    {
    }

    template <typename E>
    bool handle(EventFunction<E> eventFunction)
    {
        if (m_event.getType() == E::staticType() && !m_event.isHandled()) {
            E& e = static_cast<E&>(m_event);
            if (eventFunction(e)) e.handle();
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
