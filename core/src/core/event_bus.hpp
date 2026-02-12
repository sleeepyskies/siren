#pragma once

#include "core/core.hpp"
#include "../sync/rw_lock.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{

class EventBus {
    template <typename TEvent>
    using EventCallback = std::function<bool(TEvent&)>;
    using EventID       = usize;
    using HandlerVector = std::vector<std::function<bool(void*)>>;

    struct Event {
        EventID id;
        void* data;
        void (*dtor)(void*);

        void destroy() const { dtor(data); }
    };

    struct EventBusData {
        std::unordered_map<EventID, HandlerVector> handlers{ };
        std::queue<Event> event_queue{ };
    };

public:
    ~EventBus();

    template <typename TEvent, typename... Args>
    auto emit(Args&&... args) -> void {
        const EventID id     = get_event_type<TEvent>();
        const auto guard     = m_data.read();
        const auto& handlers = guard->handlers;
        const auto it        = handlers.find(id);
        if (it == handlers.end()) { return; }
        TEvent event{ args... };
        for (const auto& handler : it->second) {
            if (handler(&event)) { break; }
        }
    }

    template <typename TEvent, typename... Args>
    auto post(Args&&... args) -> void {
        // heap allocate the event, delete later on
        TEvent* event     = new TEvent{ std::forward<Args>(args)... };
        const auto guard  = m_data.write();
        auto& event_queue = guard->event_queue;
        event_queue.push(
            Event{
                get_event_type<TEvent>(),
                static_cast<void*>(event),
                [] (void* erased) { delete static_cast<TEvent*>(erased); }
            }
        );
    }

    auto dispatch() -> void;

    template <typename TEvent>
    auto subscribe(EventCallback<TEvent>&& callback) -> void {
        const auto guard = m_data.write();
        auto& handlers   = guard->handlers;
        handlers[get_event_type<TEvent>()].push_back(
            [callback = std::move(callback)] (void* event) {
                TEvent* casted = static_cast<TEvent*>(event);
                return callback(*casted);
            }
        );
    }

private:
    template <typename TEvent>
    auto get_event_type() const -> EventID { return std::type_index(typeid(TEvent)).hash_code(); }

    RwLock<EventBusData> m_data;
};
} // namespace siren::core
