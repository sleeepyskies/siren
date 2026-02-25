#include "core/event_bus.hpp"


namespace siren::core
{
EventBus::~EventBus() {
    const auto guard = m_inner.write();
    auto handlers    = guard->handlers;
    auto event_queue = guard->event_queue;

    handlers.clear();

    while (!event_queue.empty()) {
        const auto e = event_queue.front();
        e.destroy();
        event_queue.pop();
    }
}

auto EventBus::dispatch() -> void {
    // to avoid stalling any functions that want to emit/post events
    // while we are dispatching, we steal the event queue and release
    // the lock on it

    std::queue<Event> local_events;
    {
        const auto guard = m_inner.write();
        local_events     = std::move(guard->event_queue);
    }

    if (local_events.empty()) {
        return;
    }

    log()->trace("Dispatching {} events", local_events.size());

    const auto guard = m_inner.read();

    while (!local_events.empty()) {
        const Event event = std::move(local_events.front());
        local_events.pop();
        const auto it = guard->handlers.find(event.id);
        if (it != guard->handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event.data);
            }
        }
        event.destroy();
    }
}
} // namespace siren::core
