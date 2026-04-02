module;

#include <queue>
#include <functional>
#include <typeindex>
#include <entt/container/dense_map.hpp>

#include "assert.hpp"

export module siren.ecs.signal;

import siren.common;
import siren.sync;
import siren.log;
import siren.ecs.entity;
import siren.reflect;

namespace siren::ecs {

// forward declarations
export template <typename T>
struct Signal;

export template <typename TSignal>
class SignalBuilder;

export class SignalBus;

class World;

/**
 * @brief Collection of signal related utilities and types.
 */
struct SignalBase {
    /** @brief The type of a signals ID. */
    using IDType = HashedString::HashType;
};

/**
 * @brief Collection of templated signal related utilities and types.
 */
template <typename T>
struct Signal : SignalBase {
    /** @brief The signal ID for signals of type T. */
    static constexpr IDType ID = TypeHash<T>::hash();
};

/**
 * @brief Builder class for creating more complex signal handlers.
 * @tparam TSignal The signal type.
 */
template <typename TSignal>
class SignalBuilder {
public:
    explicit SignalBuilder(SignalBus& bus);

    /**
     * @brief Specifies the target of the signal. Signals with a target set
     * will only be triggered if the signal is emitted on this entity.
     */
    [[nodiscard]]
    auto target(const Entity& target) -> SignalBuilder&;

    /** @brief Sets the signal callback and builds the final callback. */
    template <typename Callback>
    auto run(Callback&& callback) -> void;

    // auto once() -> void;
    // auto priority(i32 priority) -> void;
    // auto when(Predicate filter) -> void;

private:
    SignalBus& m_bus;
    Entity m_target = NullEntity;
};

/// @todo: some issues may exists iff: somebody emits, which causes something else to subscribe
/// -> deadlock since we read and then write after.
/// @todo: add auto off() for when an entity is destroyed

/**
 * @class SignalBus
 * @brief A thread safe signal bus.
 *
 * Supports only immediate execution of signals.
 * Emitting a signal triggers all registered callbacks for the signal type to be called.
 */
class SignalBus {
    template <typename U>
    friend class SignalBuilder;

    /** @brief ID type of signals. */
    using SignalIDType = SignalBase::IDType;

    /** @brief Type erased signal callback. Should handle resolving the callbacks parameters. */
    using ErasedCallback = std::function<void(void*)>;

    /** @brief Mapping of entity to a list of callbacks. */
    using EntityHandlerMap = std::unordered_map<Entity, std::vector<ErasedCallback>>;

    /** @brief Mapping of Signal ID type to its @ref HandlerMap. */
    using Registry = std::unordered_map<SignalIDType, EntityHandlerMap>;

public:
    /**
     * @brief Basic constructor for a SignalBus.
     * @param world Reference to the main world this SignalBus resides in.
     */
    explicit SignalBus(World& world);

    /**
     * @brief Calls all handlers for the given signal type.
     * @tparam TSignal The signal type to emit.
     * @tparam Args The constructor arguments for the TSignal signal.
     * @param args Specific constructor arguments.
     */
    template <typename TSignal, typename... Args>
    auto emit(Args&&... args) -> void;

    /**
     * @brief Calls all handlers for the given signal type targeting a specific entity.
     * @tparam TSignal The signal type to emit.
     * @tparam Args The constructor arguments for the TSignal signal.
     * @param target The target entity.
     * @param args Specific constructor arguments.
     */
    template <typename TSignal, typename... Args>
    auto emit_to(const Entity& target, Args&&... args) -> void;

    /**
     * @brief Registers a new callback function for a specific signal type.
     * @tparam TSignal The signal type to watch and subscribe to.
     */
    template <typename TSignal>
    auto on() -> SignalBuilder<TSignal>;

    /**
     * @brief Removes signal callbacks for a given event type.
     * @tparam TSignal The signal type to clear handlers for
     * @param target The target entity to remove handlers for.
     */
    template <typename TSignal>
    auto off(const Entity& target = NullEntity) -> void;

private:
    /**
     * @brief Constructs a full signal callback.
     * @tparam TSignal The signal type.
     * @tparam Callback The callback type.
     * @param callback The signal callback.
     * @param target The signal target entity.
     */
    template <typename TSignal, IsCallable Callback>
    auto construct_callback(
        Callback&& callback,
        Entity target
    ) -> void;

    /**
     * @brief Resolves signal callback arguments.
     * @tparam ToResolve The type to resolve.
     * @tparam TSignal The signal type of the callback.
     * @return The resolved function argument.
     * @warning Crashes if invalid parameters are passed.
     */
    template <typename ToResolve, typename TSignal>
    [[nodiscard]]
    constexpr auto resolve(const TSignal& signal_instance) -> auto;

    /** @brief Registers callback handlers. */
    sync::RwLock<Registry> m_registry;
    /** @brief Cached access to the world for resolving signal callback function args. */
    World& m_world;
};


export {
    template <typename T>
    struct IsSignalBus : std::false_type { };

    template <typename T>
    struct IsSignalBus<SignalBus> : std::true_type { };

    template <typename T>
    inline constexpr bool IsSignalBus_v = IsSignalBus<T>::value;
}


// ============================================================================
// == MARK: SignalBuilder impl
// ============================================================================

template <typename TSignal>
SignalBuilder<TSignal>::SignalBuilder(SignalBus& bus) : m_bus(bus) { }

template <typename TSignal>
auto SignalBuilder<TSignal>::target(const Entity& target) -> SignalBuilder& {
    m_target = target;
    return *this;
}

template <typename TSignal>
template <typename Callback>
auto SignalBuilder<TSignal>::run(Callback&& callback) -> void {
    m_bus.construct_callback<TSignal>(std::forward<Callback>(callback), m_target);
}

// ============================================================================
// == MARK: Signalbus Impl
// ============================================================================

SignalBus::SignalBus(World& world) : m_world(world) { }

template <typename TSignal, typename... Args>
auto SignalBus::emit(Args&&... args) -> void {
    emit_to<TSignal>(NullEntity, std::forward<Args>(args)...);
}

template <typename TSignal, typename... Args>
auto SignalBus::emit_to(const Entity& target, Args&&... args) -> void {
    auto guard                     = m_registry.read();
    const auto entity_map_iterator = guard->find(Signal<TSignal>::ID);

    if (entity_map_iterator == guard->end()) {
        log::debug("Signal of type {} has not been registered yet.", TypeName<TSignal>::value());
        return;
    }

    const auto handler_vector_iterator = entity_map_iterator->second.find(target);

    if (handler_vector_iterator == guard->end()) {
        log::debug(
            "No handlers registered for an emitted event of type {} with target {}.", TypeName<TSignal>::value(),
            target.entity()
        );
        return;
    }

    TSignal signal{ args... };
    for (const auto& handler : handler_vector_iterator->second) {
        handler(&signal);
    }
}

template <typename TSignal>
auto SignalBus::on() -> SignalBuilder<TSignal> {
    return SignalBuilder<TSignal>(*this);
}

template <typename TSignal>
auto SignalBus::off(const Entity& target) -> void {
    auto guard = m_registry.write();

    const auto entity_map_iterator = guard->find(Signal<TSignal>::ID);

    if (entity_map_iterator == guard->end()) {
        return;
    }

    const auto handler_vector_iterator = entity_map_iterator->second.find(target);
    entity_map_iterator->second.erase(handler_vector_iterator);

    if (!handler_vector_iterator == guard->end()) {
        log::debug(
            "Removed {} handlers of of signal type {} for entity target {}.",
            handler_vector_iterator->second.size(),
            TypeName<TSignal>::value(),
            target.entity()
        );
    }
}

template <typename TSignal, IsCallable Callback>
auto SignalBus::construct_callback(
    Callback&& callback,
    Entity target
) -> void {
    using Traits = FunctionTraits<std::decay_t<Callback>>;
    using Args   = Traits::Args;

    auto erased_callback = [this, callback = std::move(callback)] (auto* signal_ptr) mutable {
        TSignal& signal = *static_cast<TSignal*>(signal_ptr);

        auto invoke = [&]<typename... P> (TypePack<P...>) {
            callback(this->resolve<P, TSignal>(signal)...);
        };

        invoke(Args{ });
    };

    auto guard = m_registry.write();
    (*guard)[Signal<TSignal>::ID][target].push_back(std::move(erased_callback));

    if (target == NullEntity) {
        log::debug("Added new subscriber for signals of type {}.", TypeName<TSignal>::value());
    } else {
        log::debug(
            "Added new subscriber for signals of type {} with target {}.", TypeName<TSignal>::value(), target.entity()
        );
    }
}

template <typename ToResolve, typename TSignal>
constexpr auto SignalBus::resolve(const TSignal& signal_instance) -> auto {
    using ToResolveDecayed = std::decay_t<ToResolve>;

    if constexpr (std::is_same_v<ToResolveDecayed, TSignal>) {
        // type of to resolve matches the signal
        return signal_instance;
    } else {
        // otherwise, must be something world can resolve
        return m_world.resolve<ToResolve>();
    }
}

} // namespace siren::ecs
