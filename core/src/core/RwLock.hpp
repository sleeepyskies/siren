#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

template <typename T>
using LockResult = std::expected<T, Error>;

/**
 * @class Guard
 * @brief A RAII container for accessing shared data in a thread safe manner.
 * The lock/guard is held until this object is dropped.
 * @tparam T The type of the data behind the guard.
 * @tparam Lock The type of the lock. Maybe either a shared or a unique lock.
 */
template <typename T, typename Lock>
    requires(
        std::is_same_v<Lock, std::shared_lock<std::shared_mutex>> ||
        std::is_same_v<Lock, std::unique_lock<std::shared_mutex>>
    )
class Guard {
public:
    using Pointer   = std::conditional_t<std::is_same_v<Lock, std::shared_lock<std::shared_mutex>>, const T*, T*>;
    using Reference = std::conditional_t<std::is_same_v<Lock, std::shared_lock<std::shared_mutex>>, const T&, T&>;
    using LockType  = Lock;

    explicit Guard(
        Lock&& lock,
        Reference data
    ) : m_lock(std::move(lock)), m_data(data) { }

    Guard(const Guard&)            = delete;
    Guard(Guard&&)                 = default;
    Guard& operator=(const Guard&) = delete;
    Guard& operator=(Guard&&)      = default;

    [[nodiscard]] constexpr auto operator->() noexcept -> Pointer { return &m_data; }
    [[nodiscard]] constexpr auto operator->() const noexcept -> Pointer { return &m_data; }
    [[nodiscard]] constexpr auto operator*() noexcept -> Reference { return m_data; }
    [[nodiscard]] constexpr auto operator*() const noexcept -> Reference { return m_data; }

private:
    LockType m_lock;  ///< @brief The lock on the data.
    Reference m_data; ///< @brief Reference to the data.
};

/**
 * @class RwLock
 * @brief A thread safe container allowing multiple threads read and write
 * access to some shared resource.
 * @tparam T The wrapped resource.
 */
template <typename T>
class RwLock {
public:
    using ReadGuard  = Guard<T, std::shared_lock<std::shared_mutex>>;
    using WriteGuard = Guard<T, std::unique_lock<std::shared_mutex>>;

    RwLock() : m_data(T()) { }
    template <typename... Args>
    explicit RwLock(Args&&... args) : m_data(T(std::move(args...))) { }
    explicit RwLock(T&& t) : m_data(std::move(t)) { }

    RwLock(const RwLock&)            = delete;
    RwLock(RwLock&&)                 = default;
    RwLock& operator=(const RwLock&) = delete;
    RwLock& operator=(RwLock&&)      = default;

    /// @brief Perform a blocking read. If the resource is currently
    /// locked with a write, the thread will wait until it is freed.
    [[nodiscard]]
    auto read() const -> ReadGuard {
        typename ReadGuard::LockType lock{ m_mutex }; // blocking
        return ReadGuard{ std::move(lock), m_data };
    }

    /// @brief Perform a non-blocking read.
    [[nodiscard]]
    auto try_read() const -> LockResult<ReadGuard> {
        typename ReadGuard::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return Error(Code::ResourceLocked); }
        return ReadGuard{ std::move(lock), m_data };
    }

    /// @brief Perform a blocking write. If the resource is
    /// currently locked, the thread will wait until it is free.
    [[nodiscard]]
    auto write() -> WriteGuard {
        typename WriteGuard::LockType lock{ m_mutex }; // blocking
        return WriteGuard{ std::move(lock), m_data };
    }

    /// @brief Perform a non-blocking write.
    [[nodiscard]]
    auto try_write() -> LockResult<WriteGuard> {
        typename WriteGuard::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return Error(Code::ResourceLocked); }
        return WriteGuard{ std::move(lock), m_data };
    }

private:
    T m_data;                          ///< @brief The underlying guarded data.
    mutable std::shared_mutex m_mutex; ///< @brief Resource mutex.
};

} // namespace siren::core
