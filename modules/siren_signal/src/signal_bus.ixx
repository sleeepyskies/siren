module;

#include <queue>
#include <functional>
#include <typeindex>

export module siren.signal;

import siren.common;
import siren.sync;
import siren.log;

namespace siren::signal {

/// @brief Function type signature for a signal callback. Returns true to consume the signal.
export template <typename TSignal>
using SignalCallback = std::function<void(TSignal&)>;

/**
 * @class SignalBus
 * @brief A thread safe signal bus.
 *
 * Supports only immediate execution of signals. For deferred execution, see
 * siren_event
 */
export class SignalBus {

    using SignalID      = usize;
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
            if (handler(&signal)) { break; }
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
    auto get_signal_type() const -> SignalID { return std::type_index(typeid(TSignal)).hash_code(); }

    /// @brief Registers callback handlers.
    sync::RwLock<std::unordered_map<SignalID, HandlerVector>> m_handlers;
};

} // namespace siren::signal
