#include "EventBus.hpp"


namespace siren::core
{
EventBus::~EventBus()
{
    m_handlers.clear();
    while (!m_eventQueue.empty()) {
        const auto e = m_eventQueue.front();
        e.destroy();
        m_eventQueue.pop();
    }
}

void EventBus::Dispatch()
{
    while (!m_eventQueue.empty()) {
        const Event event = std::move(m_eventQueue.front());
        m_eventQueue.pop();
        const auto it = m_handlers.find(event.id);
        if (it != m_handlers.end()) {
            for (const auto& handler : it->second) {
                if (handler(event.data)) { break; }
            }
        }
        event.destroy();
    }
}
} // namespace siren::core
