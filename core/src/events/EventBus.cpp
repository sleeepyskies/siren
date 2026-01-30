#include "EventBus.hpp"


namespace siren::core
{
EventBus::~EventBus() {
    const auto guard = m_data.write();
    auto handlers    = guard->handlers;
    auto event_queue = guard->event_queue;

    handlers.clear();

    while (!event_queue.empty()) {
        const auto e = event_queue.front();
        e.destroy();
        event_queue.pop();
    }
}

void EventBus::dispatch() {
    const auto guard = m_data.write();
    auto handlers    = guard->handlers;
    auto event_queue = guard->event_queue;

    while (!event_queue.empty()) {
        const Event event = std::move(event_queue.front());
        event_queue.pop();
        const auto it = handlers.find(event.id);
        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                if (handler(event.data)) { break; }
            }
        }
        event.destroy();
    }
}
} // namespace siren::core
