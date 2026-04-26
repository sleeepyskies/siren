module;

#include <expected>
#include <shared_mutex>
#include <string_view>
#include <utility>

export module siren.sync:rw_lock;

import :guard;
import siren.common;

namespace siren::sync {

/** @brief Error code for the @ref RwLock. */
export enum class RwLockErrorCode {
    ResourceLocked,
};

/** @brief To string method for @ref RwLockErrorCode. */
constexpr auto to_string(const RwLockErrorCode code) -> std::string_view {
    switch (code) {
        case RwLockErrorCode::ResourceLocked: return "ResourceLocked";
        default: std::unreachable();
    }
}

/** @brief @ref siren::Error specialization for the @ref Mutex class. */
export using RwLockError = Error<RwLockErrorCode>;

/** @brief expected specialization for the @ref RwLock. */
export template <typename T>
using RwLockExpected = std::expected<T, RwLockError>;

/**
 * @class RwLock
 * @brief A thread safe container allowing multiple threads read and write
 * access to some shared resource.
 * @details This lock allows any number of readers to acquire a lock on
 * the wrapped resource, but only one writer to have a lock.
 * @tparam T The wrapped resource.
 */
export template <typename T>
class RwLock {
public:
    RwLock() : m_data(T()) { }
    template <typename... Args>
    explicit RwLock(Args&&... args) : m_data(T(std::move(args...))) { }
    explicit RwLock(T&& t) : m_data(std::move(t)) { }

    RwLock(const RwLock&)            = delete;
    RwLock(RwLock&&)                 = delete;
    RwLock& operator=(const RwLock&) = delete;
    RwLock& operator=(RwLock&&)      = delete;

    /**
     * @brief Perform a blocking read. If the resource is currently
     * locked with a Write, the thread will wait until it is freed.
     */
    [[nodiscard]]
    auto read() const -> ReadGuard<T> {
        typename ReadGuard<T>::LockType lock{ m_mutex }; // blocking
        return ReadGuard<T>{ std::move(lock), m_data };
    }

    /** @brief Attempts to obtain a @ref ReadGuard. Returns std::unexpected on failure. */
    [[nodiscard]]
    auto try_read() const -> RwLockExpected<ReadGuard<T>> {
        typename ReadGuard<T>::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return std::unexpected{ RwLockErrorCode::ResourceLocked }; }
        return ReadGuard<T>{ std::move(lock), m_data };
    }

    /**
     * @brief Perform a blocking write. If the resource is
     * currently locked, the thread will wait until it is free.
     */
    [[nodiscard]]
    auto write() -> WriteGuard<T> {
        typename WriteGuard<T>::LockType lock{ m_mutex }; // blocking
        return WriteGuard<T>{ std::move(lock), m_data };
    }

    /** @brief Attempts to obtain a @ref WriteGuard. Returns std::unexpected on failure. */
    [[nodiscard]]
    auto try_write() -> RwLockExpected<WriteGuard<T>> {
        typename WriteGuard<T>::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) { return std::unexpected{ RwLockErrorCode::ResourceLocked }; }
        return WriteGuard<T>{ std::move(lock), m_data };
    }

    /**
     * @brief Runs the given function with a ReadGuard.
     * @warning May block the thread.
     * @tparam Function The function type to run.
     * @param func The function instance to call with a ReadGuard.
     */
    template <typename Function>
    auto read_scoped(Function&& func) const noexcept -> void {
        auto guard = this->read();
        func(guard);
    }

    /**
     * @brief Tries to run the given function with a ReadGuard.
     * @tparam Function The function type to run.
     * @param func The function instance to call with a ReadGuard.
     */
    template <typename Function>
    auto try_read_scoped(Function&& func) const noexcept -> RwLockExpected<void> {
        auto result = this->try_read();
        if (result.has_value()) {
            func(result.value());
            return { };
        }
        return result;
    }

    /**
     * @brief Runs the given function with a WriteGuard.
     * @warning May block the thread.
     * @tparam Function The function type to run.
     * @param func The function instance to call with a WriteGuard.
     */
    template <typename Function>
    auto write_scoped(Function&& func) const noexcept -> void {
        auto guard = this->write();
        func(guard);
    }

    /**
     * @brief Tries to run the given function with a WriteGuard.
     * @tparam Function The function type to run.
     * @param func The function instance to call with a WriteGuard.
     */
    template <typename Function>
    auto try_write_scoped(Function&& func) const noexcept -> RwLockExpected<void> {
        auto result = this->try_write();
        if (result.has_value()) {
            func(result.value());
            return { };
        }
        return result;
    }

    /**
     * @brief Sets the inner value of the RwLock.
     * @warning Performs a block, and thus may stall the thread.
     */
    template <typename U>
    auto set(U&& val) noexcept -> void { *write() = std::forward<U>(val); }

    /**
     * @brief Returns a copy of the inner value of the RwLock.
     * @warning May stall the thread if the RwLock is locked for writing when called.
     */
    [[nodiscard]]
    auto get() const noexcept -> T { return *read(); }

    /**
     * @brief Locks the resource and returns and consumes the inner value.
     * @warning After calling this, the inner value will have its default state.
     * @warning May stall the current thread.
     */
    [[nodiscard]]
    auto consume() noexcept -> T {
        auto guard = write();
        return std::exchange(m_data, T{ });
    }

private:
    /** @brief The underlying guarded data. */
    T m_data;
    /** @brief Resource mutex. */
    mutable std::shared_mutex m_mutex;
};

export template <typename T>
using MoveableRwLock = std::unique_ptr<RwLock<T>>;

} // namespace siren::core
