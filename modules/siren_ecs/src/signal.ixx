module;

#include <queue>
#include <functional>
#include <typeindex>
#include <entt/container/dense_map.hpp>

export module siren.ecs.signal;

import siren.common;
import siren.sync;
import siren.log;
import siren.ecs.entity;
import siren.reflect;

namespace siren::ecs {

/// @todo: add entity specific listeners for signals
/// @todo: some issues may exists iff: somebody emits, which causes something else to subscribe
/// -> deadlock since we read and then write after.

/// @brief Function type signature for a signal callback. Returns true to consume the signal.
export template <typename TSignal>
using SignalCallback = std::function<void(TSignal&)>;

/**
 * @class SignalBus
 * @brief A thread safe signal bus.
 *
 * Supports only immediate execution of signals.
 * Emitting a signal triggers all registered callbacks for the signal type to be called.
 */
export class SignalBus {
    using SignalID      = HashedString::HashType;
    using HandlerVector = std::vector<std::function<void(void*)>>;

    /**
     * @struct Signal
     * @brief Internal type erased signal representation.
     */
    struct Signal {
        /// @brief Some unique signal identifier.
        SignalID id;
        /// @brief Heap allocated signal data.
        void* data;
        /// @brief A simple function pointer to a destructor for the signal.
        void (*dtor)(void*);

        /// @brief Helper method to call the dtor on this signal's data.
        void destroy() const { dtor(data); }
    };

public:
    SignalBus()  = default;
    ~SignalBus() = default;

    /**
     * @brief Calls all handlers for the given signal type.
     * @tparam TSignal The signal type to emit.
     * @tparam Args The constructor arguments for the TSignal signal.
     * @param args Specific constructor arguments.
     */
    template <typename TSignal, typename... Args>
    auto emit(Args&&... args) -> void {
        auto guard    = m_handlers.read();
        const auto it = guard->find(get_signal_type<TSignal>());

        if (it == guard->end()) {
            log::debug("No handlers registered for an emitted event of type {}.", TypeName<TSignal>::value());
            return;
        }

        TSignal signal{ args... };
        for (const auto& handler : it->second) {
            handler(&signal);
        }
    }

    /**
     * @brief Registers a new callback function for a specific signal type.
     * @tparam TSignal The signal type to watch and subscribe to.
     * @param callback The signal call back handler.
     */
    template <typename TSignal>
    auto subscribe(SignalCallback<TSignal>&& callback) -> void {
        auto guard = m_handlers.write();
        (*guard)[get_signal_type<TSignal>()].push_back(
            [callback = std::move(callback)] (void* signal) {
                TSignal* casted = static_cast<TSignal*>(signal);
                return callback(*casted);
            }
        );
        log::debug("Added new subscriber for signals of type: {}", TypeName<TSignal>::value());
    }

private:
    /// @brief Simple helper function to retrieve the @ref SignalID of some TSignal.
    template <typename TSignal>
    auto get_signal_type() const -> SignalID { return TypeHash<TSignal>::value().hash(); }

    /// @brief Registers callback handlers.
    sync::RwLock<entt::dense_map<SignalID, HandlerVector>> m_handlers;
};


export {
    template <typename T>
    struct IsSignalBus : std::false_type { };

    template <typename T>
    struct IsSignalBus<SignalBus> : std::true_type { };

    template <typename T>
    inline constexpr bool IsSignalBus_v = IsSignalBus<T>::value;
}

} // namespace siren::ecs
