module;

#include <mutex>
#include <expected>
#include <string_view>
#include <utility>
#include <functional>

export module siren.sync:mutex;

import :guard;
import siren.common;

namespace siren::sync {

/** @brief Error code for the @ref Mutex class. */
export enum class MutexErrorCode {
    ResourceLocked,
};

/** @brief To string method for @ref MutexErrorCode. */
constexpr auto to_string(const MutexErrorCode code) -> std::string_view {
    switch (code) {
        case MutexErrorCode::ResourceLocked: return "ResourceLocked";
        default: std::unreachable();
    }
}

/** @brief @ref siren::Error specialization for the @ref Mutex class. */
export using MutexError = Error<MutexErrorCode>;

/** @brief expected specialization for the @ref Mutex. */
export template <typename T>
using MutexExpected = std::expected<T, MutexError>;

/**
 * @brief A thread safe RAII container allowing multiple threads
 * to access a shared resource.
 *
 * @details Mutex allows only one thread access at a time. This
 * applies for both read and write access. Attempting access to
 * a locked resource will either block or crash the thread,
 * depending on how this was done (aka either lock() try_lock()).
 *
 *
 * @tparam T The wrapped resource type.
 *
 * @see Mutex
 * @see Guard
 */
export template <typename T>
class Mutex {
public:
    Mutex() : m_data(T()) { }
    template <typename... Args>
    explicit Mutex(Args... args) : m_data(std::forward<Args>(args)...) { }
    explicit Mutex(T&& t) : m_data(std::move(t)) { }

    Mutex(const Mutex&)            = delete;
    Mutex(Mutex&&)                 = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator=(Mutex&&)      = delete;

    /** @brief Obtains a blocking guard. If the resource is
     * currently locked, the thread will wait until it is free.
     */
    [[nodiscard]]
    auto lock() const noexcept -> UniqueGuard<T> {
        typename UniqueGuard<T>::LockType lock{ m_mutex }; // blocking
        return UniqueGuard<T>{ std::move(lock), m_data };
    }

    /** @brief Attempts to obtain a @ref UniqueGuard. Returns std::unexpected on failure. */
    [[nodiscard]]
    auto try_lock() const noexcept -> MutexExpected<UniqueGuard<T>> {
        typename UniqueGuard<T>::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return std::unexpected(MutexErrorCode::ResourceLocked); }
        return UniqueGuard<T>{ std::move(lock), m_data };
    }

    /**
     * @brief Runs the given lambda immediately passing in a locked @ref Guard as
     * an argument.
     * This function is essentially a helper to perform some scoped action with a lock.
     * @tparam Function A lambda that takes the guard as an argument.
     */
    template <typename Function>
        requires(std::is_invocable_v<Function, UniqueGuard<T>&>)
    auto run_scoped(
        Function&& func
    ) const noexcept -> std::invoke_result_t<Function, UniqueGuard<T>&> {
        auto guard = lock();
        return std::invoke(std::forward<Function>(func), guard);
    }

    /**
     * @brief Helper function to run try to run a lambda function with the guard.
     * @tparam Function A lambda that takes the guard as an argument.
     * @return @ref MutexExpected with void on success, and @ref Error on fail.
     */
    template <typename Function>
    [[nodiscard]]
    auto try_scoped(
        Function&& func
    ) const noexcept -> MutexExpected<std::invoke_result_t<Function, UniqueGuard<T>>> {
        auto result = this->try_lock();
        if (result.has_value()) {
            return std::invoke(std::forward<Function>(func), result.value());
        }
        return result;
    }

    /**
     * @brief Sets the inner value of the mutex.
     * @warning Performs a block, and thus may stall the thread.
     */
    template <typename U>
    auto set(U&& val) noexcept -> void { *lock() = std::forward<U>(val); }

    /**
     * @brief Returns a copy of the inner value of the mutex.
     * @warning May stall the thread if the mutex is locked for writing when called.
     */
    [[nodiscard]]
    auto get() const noexcept -> T { return *lock(); }

    /**
     * @brief Locks the resource and returns and consumes the inner value.
     * @warning After calling this, the inner value will have its default state.
     * @warning May stall the current thread.
     */
    [[nodiscard]]
    auto consume() noexcept -> T {
        auto guard = lock();
        return std::exchange(m_data, T{ });
    }

private:
    /** @brief The underlying guarded data. */
    mutable T m_data;
    /** @brief Resource mutex. */
    mutable std::mutex m_mutex;
};

} // namespace siren::sync
