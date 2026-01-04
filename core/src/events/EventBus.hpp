#pragma once

#include <functional>
#include "utilities/spch.hpp"


namespace siren::core
{
class EventBus
{
public:
    template <typename TEvent>
    using EventCallback = std::function<bool(TEvent&)>;
    using EventID       = size_t;

    ~EventBus();

    template <typename TEvent, typename... Args>
    void Emit(Args&&... args)
    {
        const EventID id = getEventType<TEvent>();
        const auto it    = m_handlers.find(id);
        if (it == m_handlers.end()) { return; }
        TEvent event{ args... };
        for (const auto& handler : it->second) {
            if (handler(&event)) { break; }
        }
    }

    template <typename TEvent, typename... Args>
    void Post(Args&&... args)
    {
        // heap allocate the event, delete later on
        TEvent* event = new TEvent{ std::forward<Args>(args)... };
        m_eventQueue.push(
            Event{
                getEventType<TEvent>(),
                static_cast<void*>(event),
                [] (void* erased) { delete static_cast<TEvent*>(erased); }
            }
        );
    }

    void Dispatch();

    template <typename TEvent>
    void Subscribe(EventCallback<TEvent>&& callback)
    {
        m_handlers[getEventType<TEvent>()].push_back(
            [callback = std::move(callback)] (void* event) {
                TEvent* casted = static_cast<TEvent*>(event);
                return callback(*casted);
            }
        );
    }

private:
    struct Event
    {
        EventID id;
        void* data;
        void (*dtor)(void*);

        void destroy() const { dtor(data); }
    };

    template <typename TEvent>
    EventID getEventType() const { return std::type_index(typeid(TEvent)).hash_code(); }

    HashMap<EventID, Vector<std::function<bool(void*)>>> m_handlers{ };

    std::queue<Event> m_eventQueue{ };
};
} // namespace siren::core
