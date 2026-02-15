#pragma once

#include <mutex>

#include "guard.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

/// @brief The guard type returned by a @ref Mutex.
template <typename T>
using UniqueGuard = Guard<T, std::unique_lock<std::mutex>>;

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
template <typename T>
class Mutex {
public:
    Mutex() : m_data(T()) { }
    template <typename... Args>
    explicit Mutex(Args... args) : m_data(T(std::move(args...))) { }
    explicit Mutex(T&& t) : m_data(std::move(t)) { }

    Mutex(const Mutex&)             = delete;
    Mutex(Mutex&&)                  = delete;
    Mutex& operator=(const Mutex&)  = delete;
    Mutex& operator=(const Mutex&&) = delete;

    /// @brief Obtains a blocking guard. If the resource is
    /// currently locked, the thread will wait until it is free.
    [[nodiscard]]
    auto lock() -> UniqueGuard<T> {
        typename UniqueGuard<T>::LockType lock{ m_mutex }; // blocking
        return UniqueGuard<T>{ std::move(lock), m_data };
    }

    /// @brief Attempts to obtain a @ref UniqueGuard. Returns std::unexpected on failure.
    [[nodiscard]]
    auto try_lock() -> std::expected<UniqueGuard<T>, Error> {
        typename UniqueGuard<T>::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return std::unexpected(Error(Code::ResourceLocked)); }
        return UniqueGuard<T>{ std::move(lock), m_data };
    }

    /// @brief Helper function to run a blocking lambda function with the guard.
    /// @tparam Function A lambda that takes the guard as an argument.
    template <typename Function>
    auto run_scoped(Function&& func) -> void {
        auto guard = this->lock();
        func(guard);
    }

    /// @brief Helper function to run try to run a lambda function with the guard.
    /// @tparam Function A lambda that takes the guard as an argument.
    /// @return std::expected with void on success, and @ref Error on fail.
    template <typename Function>
    auto try_run_scoped(Function&& func) -> std::expected<void, Error> {
        auto result = this->try_lock();
        if (result.has_value()) {
            func(result.value());
        }
    }

private:
    T m_data;                   ///< @brief The underlying guarded data.
    mutable std::mutex m_mutex; ///< @brief Resource mutex.
};
} // namespace siren::core
