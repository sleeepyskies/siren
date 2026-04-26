module;

#include <queue>
#include <functional>
#include <vector>
#include <typeinfo>
#include <new>

export module siren.ecs:signals;

import :entity;

import siren.common;
import siren.sync;
import siren.log;
import siren.reflect;

namespace siren::ecs {

/// @todo: some issues may exists iff: somebody emits, which causes something else to subscribe
/// -> deadlock since we read and then write after.

/**
 * @class SignalBus
 * @brief A thread safe signal bus.
 *
 * Supports only immediate execution of signals.
 * Emitting a signal triggers all registered callbacks for the signal type to be called.
 */
export class SignalBus {
public:
    /** @brief ID type of signals. */
    using SignalID = HashedString::HashType;

    /** @brief Type erased signal callback. Should handle resolving the callbacks parameters. */
    using Callback = std::function<void(void*)>;

    /** @brief Constructs a new Signals. */
    SignalBus()                            = default;
    SignalBus(const SignalBus&)            = delete;
    SignalBus(SignalBus&&)                 = default;
    SignalBus& operator=(const SignalBus&) = delete;
    SignalBus& operator=(SignalBus&&)      = default;

    /** @brief Returns the SignalID of a give signal type. */
    template <typename TSignal>
    static constexpr auto id() -> SignalID { return TypeHash<TSignal>::hash(); }

    /**
     * @brief Calls all handlers for the given signal type.
     * @tparam TSignal The signal type to emit.
     * @tparam Args The constructor arguments for the TSignal signal.
     * @param args Specific constructor arguments.
     */
    template <typename TSignal, typename... Args>
    auto emit(Args&&... args) -> void {
        emit_to<TSignal>(NullEntity, std::forward<Args>(args)...);
    }

    /**
     * @brief Calls all handlers for the given signal type targeting a specific entity.
     * @tparam TSignal The signal type to emit.
     * @tparam Args The constructor arguments for the TSignal signal.
     * @param target The target entity.
     * @param args Specific constructor arguments.
     */
    template <typename TSignal, typename... Args>
    auto emit_to(const Entity& target, Args&&... args) -> void {
        auto guard = m_registry.read();

        const auto signal_it = guard->find(typehash_of<TSignal>());

        if (signal_it == guard->end()) {
            log::debug("Signal of type {} has not been registered yet.", typename_of<TSignal>());
            return;
        }

        auto& entity_map     = signal_it->second;
        const auto entity_it = entity_map.find(target);

        if (entity_it == entity_map.end()) {
            log::debug(
                "No handlers registered for an emitted event of type {} with target {}.", TypeName<TSignal>::value(),
                target.value()
            );
            return;
        }

        TSignal signal{ std::forward<Args>(args)... };
        for (const auto& handler : entity_it->second) {
            handler(&signal);
        }
    }

    /**
     * @brief Registers a new signal subscriber.
     * @tparam TSignal The SignalID to listen for.
     * @param callback The callback to call once the signal is triggered.
     * @param target The target entity. May be NullEntity for no target.
     */
    template <typename TSignal>
    auto on(Callback&& callback, const Entity target = NullEntity) -> void {
        auto guard = m_registry.write();
        (*guard)[id<TSignal>()][target].push_back(std::forward<Callback>(callback));
        log_on<TSignal>(target);
    }

    /**
     * @brief Removes signal callbacks for a given event type.
     * @tparam TSignal The SignalID to listen for.
     * @param target The target entity to remove handlers for.
     */
    template <typename TSignal>
    auto off(const Entity target = NullEntity) -> void {
        auto guard = m_registry.write();

        const auto signal_it = guard->find(id<TSignal>());
        if (signal_it == guard->end()) { return; }

        auto& entity_map     = signal_it->second;
        const auto entity_it = entity_map.find(target);

        if (entity_it == entity_map.end()) { return; }

        log_off<TSignal>(entity_it->second.size(), target);
        entity_map.erase(entity_it);
    }

private:
    /** @brief Mapping of entity to a list of callbacks. */
    using EntityCallbacks = std::unordered_map<Entity, std::vector<Callback>>;
    /** @brief Mapping of Signal ID type to its @ref HandlerMap. */
    using Registry = std::unordered_map<SignalID, EntityCallbacks>;
    /** @brief Registers callback handlers. */
    sync::RwLock<Registry> m_registry;

    template <typename TSignal>
    constexpr auto log_on(const Entity target) const noexcept {
        constexpr auto sname = typename_of<TSignal>();
        if (target == NullEntity) {
            log::trace("Registered global signal listener for {}", sname);
        } else {
            log::trace("Registered signal listener for {} with target {}", sname, target.value());
        }
    }

    template <typename TSignal>
    constexpr auto log_off(const u32 count, const Entity target) const noexcept {
        constexpr auto sname = typename_of<TSignal>();
        if (target == NullEntity) {
            log::debug("Removed {} listeners for signal {}.", count, sname);
        } else {
            log::debug("Removed {} listeners for signal {} targeting {}.", count, sname, target.value());
        }
    }
};


export {
    template <typename T>
    struct IsSignals : std::false_type { };

    template <>
    struct IsSignals<SignalBus> : std::true_type { };

    template <typename T>
    inline constexpr bool IsSignals_v = IsSignals<T>::value;
}

} // namespace siren::ecs
