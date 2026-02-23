#pragma once

#include "core/core.hpp"
#include "sync/rw_lock.hpp"

#include "spch.hpp"


namespace siren::core
{

/// @todo The event bus is nice, but maybe we could use a Reader Writer system?

/**
 * @class EventBus
 * @brief A thread safe event bus.
 *
 * Supports both immediate execution and deferred.
 */
class EventBus : WithLogger<SystemLogger::Core> {
    /// @brief Function type signature for an event callback. Returns true to consume the event.
    template <typename TEvent>
    using EventCallback = std::function<bool(TEvent&)>;

    using EventID       = usize;
    using HandlerVector = std::vector<std::function<bool(void*)>>;

    /**
     * @struct Event
     * @brief Internal type erased event representation.
     */
    struct Event {
        /// @brief Some unique event identifier.
        EventID id;
        /// @brief Heap allocated event data.
        void* data;
        /// @brief A simple function pointer to a destructor for the event.
        void (*dtor)(void*);

        /// @brief Helper method to call the dtor on this events data.
        void destroy() const { dtor(data); }
    };

    /**
     * @brief Internal data of the @ref EventBus.
     */
    struct Inner {
        std::unordered_map<EventID, HandlerVector> handlers{ };
        std::queue<Event> event_queue{ };
    };

public:
    ~EventBus();

    /**
     * @brief Immediately calls all handlers of this event type.
     * @tparam TEvent The event type to emit.
     * @tparam Args The constructor arguments for the TEvent event.
     * @param args Specific constructor arguments.
     */
    template <typename TEvent, typename... Args>
    auto emit(Args&&... args) -> void {
        const EventID id     = get_event_type<TEvent>();
        const auto guard     = m_inner.read();
        const auto& handlers = guard->handlers;
        const auto it        = handlers.find(id);
        if (it == handlers.end()) {
            log()->debug(
                "Event emitted with no handlers. Event Type: {}. EventID: {}", TypeName<TEvent>::value(), id
            );
            return;
        }
        TEvent event{ args... };
        for (const auto& handler : it->second) {
            if (handler(&event)) { break; }
        }
    }

    /**
     * @brief Queues an event for deferred handling.
     * @tparam TEvent The event type to post.
     * @tparam Args The constructor arguments for the TEvent event.
     * @param args Specific constructor arguments.
     */
    template <typename TEvent, typename... Args>
    auto post(Args&&... args) -> void {
        // heap allocate the event, delete later on
        TEvent* event = new TEvent{ std::forward<Args>(args)... };
        auto guard    = m_inner.write();
        guard->event_queue.push(
            Event{
                get_event_type<TEvent>(),
                static_cast<void*>(event),
                [] (void* erased) { delete static_cast<TEvent*>(erased); }
            }
        );
    }

    /**
     * @brief Registers a new callback function for a specific event type.
     * @tparam TEvent The event type to watch and subscribe to.
     * @param callback The event call back handler.
     */
    template <typename TEvent>
    auto subscribe(EventCallback<TEvent>&& callback) -> void {
        auto guard = m_inner.write();
        guard->handlers[get_event_type<TEvent>()].push_back(
            [callback = std::move(callback)] (void* event) {
                TEvent* casted = static_cast<TEvent*>(event);
                return callback(*casted);
            }
        );
        log()->debug("Added new subscriber for events of type: {}", TypeName<TEvent>::value());
    }

    /**
     * @brief Handles all events currently in the queue.
     */
    auto dispatch() -> void;

private:
    /// @brief Simple helper function to retrieve the @ref EventID of some TEvent.
    template <typename TEvent>
    auto get_event_type() const -> EventID { return std::type_index(typeid(TEvent)).hash_code(); }

    /// @brief The inner data of the @ref EventBus locked
    /// behind a @ref RwLock for thread safety.
    RwLock<Inner> m_inner;
};
} // namespace siren::core
