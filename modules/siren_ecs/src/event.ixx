module;

#include <ranges>
#include <vector>
#include <unordered_map>
#include <memory>

export module siren.ecs:event;

import siren.sync;
import siren.common;
import siren.reflect;
import siren.log;

namespace siren::ecs {

/** @brief ID type for events. */
using EventID = HashedString::HashType;

/** @brief Event Wrapper providing a compile time EventID. */
template <typename TEvent>
class Event {
    /** @brief EventID determined at compile time. */
    static constexpr EventID ID = HashedString{ TypeName<TEvent>::value() }.hash();
};

export class EventBus;

/** @brief Simple parent class for polymorphism. */
class EventBufferBase {
    friend class EventBus;

public:
    virtual ~EventBufferBase() = 0;

private:
    virtual auto swap_buffers() -> void = 0;
};

/**
 * @brief A threadsafe event buffer.
 * @tparam TEvent The type of event held in this buffer.
 * The EventBuffer manages two buffers internally. This is to avoid situations
 * where calling event_reader.view() two times in a frame would provide different
 * results due to the buffer being written to that frame.
 * Therefore, all events available to be read, are from the previous frame.
 */
export template <typename TEvent>
class EventBuffer final : EventBufferBase {

public:
    EventBuffer()           = default;
    ~EventBuffer() override = default;

    /**
     * @brief Adds a new element of type TEvent to this buffer.
     * @param event The event instance to push into the buffer.
     */
    auto push(const TEvent& event) -> void {
        m_this_frame.run_scoped(
            [&event] (sync::UniqueGuard<std::vector<TEvent>>& guard) { guard->push_back(event); }
        );
    }

    /**
     * @brief Adds a new element of type TEvent to this buffer inplace.
     * @tparam Args The type of the pack needed to construct a TEvent.
     * @param args The arguments used to construct the event.
     */
    template <typename... Args>
    auto emplace(Args&&... args) -> void {
        m_this_frame.run_scoped(
            [...args = std::forward<Args>(args)] (sync::UniqueGuard<std::vector<TEvent>>& guard) {
                guard->emplace_back(std::forward<Args>(args)...);
            }
        );
    }

    /**
     * @brief Returns a copy of the internally held buffer.
     * @note The returned events are the events of last frame.
     */
    auto view() -> const std::vector<TEvent>& {
        return m_last_frame;
    }

private:
    /** @brief The events coming in this frame. */
    sync::Mutex<std::vector<TEvent>> m_this_frame;
    /** @brief The events from last frame. No sync primitives since we read only during a frame. */
    std::vector<TEvent> m_last_frame;

    /**
     * @brief Swaps the internally held buffers. Should be called only once per frame.
     * @note Only to be called from the @ref EventBus.
     */
    auto swap_buffers() -> void override {
        m_last_frame = m_this_frame.consume();
        m_this_frame.set({ });
    }
};

/**
 * @class EventBus
 * @brief Simple manager for
 */
class EventBus {
public:
    EventBus()                           = default;
    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&)                 = default;
    EventBus& operator=(EventBus&&)      = default;

    /**
     * @brief Swaps the internal buffers of all @ref EventBuffer's.
     * @note Make sure to call this once and only once per frame.
     */
    auto swap_event_buffers() -> void {
        for (const auto& buffer : m_buffers | std::ranges::views::values) {
            buffer->swap_buffers();
        }
    }

    /**
     * @brief Registers an event type from the bus.
     * @tparam TEvent The event type to register.
     * @note If the event type already exists, nothing happens.
     */
    template <typename TEvent>
    auto register_event() -> void {
        auto pair = m_buffers.insert(Event<TEvent>::ID, std::make_unique<EventBuffer<TEvent>>());
        if (pair.second) {
            log::debug("Registered new event type {}", TypeName<TEvent>::value());
        }
    }

    /**
     * @brief Removes an event type from the bus.
     * @tparam TEvent The event type to remove.
     */
    template <typename TEvent>
    auto remove_event() -> void {
        auto it = m_buffers.erase(Event<TEvent>::ID);
        if (it != m_buffers.end()) {
            log::debug("Removed event type {}", TypeName<TEvent>::value());
        }
    }

    /**
     * @brief Checks if the given event is registered or not.
     * @tparam TEvent The event type to check
     * @return `true` if TEvent is registered, false otherwise.
     */
    template <typename TEvent>
    auto has_event() -> bool {
        return m_buffers.contains(Event<TEvent>::ID);
    }

    /**
     * @brief Returns the registered buffer for the given event type.
     * @tparam TEvent The event type to return a buffer for.
     * @return The buffer for the event type.
     * @warning Crashes if the event type has not been registered.
     */
    template <typename TEvent>
    auto event_buffer() -> EventBuffer<TEvent>& {
        return m_buffers.at(Event<TEvent>::ID);
    }

    template <typename TEvent>
    auto push(TEvent&& event) -> void {
        return event_buffer<TEvent>()->push(std::forward<TEvent>(event));
    }

private:
    /** @brief Mapping of an event type ID to its buffer. */
    std::unordered_map<EventID, std::unique_ptr<EventBufferBase>> m_buffers;
};


export {
    /** @brief Type trait to check if a type is an @ref EventBuffer. */
    template <typename T>
    struct IsEventBuffer : std::false_type { };

    /** @brief IsEventBuffer specialization for @ref EventBuffer. */
    template <typename T>
    struct IsEventBuffer<EventBuffer<T>> : std::true_type { };

    /** @brief Helper constant for IsEventBuffer. @see IsEventBuffer. */
    template <typename T>
    inline constexpr bool IsEventBuffer_v = IsEventBuffer<T>::value;

    /** @brief Template helper for extracting meta-data about @ref EventBuffer's. */
    template <typename T>
    struct EventTraits { };

    /** @brief EventTraits specialization for @ref EventBuffer. */
    template <typename T>
    struct EventTraits<EventBuffer<T>> {
        using EventType = T;
    };
}

} // namespace siren::ecs



