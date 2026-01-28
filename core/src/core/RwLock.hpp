#pragma once

#include <shared_mutex>
#include "Result.hpp"


namespace siren::core
{

template <typename T>
using LockResult = Result<T, Error>;

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
        T& data
    ) : m_lock(std::move(lock)), m_data(data) { }

    [[nodiscard]] constexpr auto operator->() noexcept -> Pointer { return &m_data; }
    [[nodiscard]] constexpr auto operator->() const noexcept -> Pointer { return &m_data; }
    [[nodiscard]] constexpr auto operator*() noexcept -> Reference { return m_data; }
    [[nodiscard]] constexpr auto operator*() const noexcept -> Reference { return m_data; }

private:
    LockType m_lock;
    T& m_data;
};

template <typename T>
class RwLock {
public:
    using ReadGuard  = Guard<T, std::shared_lock<std::shared_mutex>>;
    using WriteGuard = Guard<T, std::unique_lock<std::shared_mutex>>;

    RwLock() : m_data(T()) { }
    template <typename... Args>
    explicit RwLock(Args&&... args) : m_data(T(std::move(args...))) { }
    explicit RwLock(T&& t) : m_data(std::move(t)) { }

    [[nodiscard]]
    auto read() -> ReadGuard {
        typename ReadGuard::LockType lock{ m_mutex }; // blocking
        return ReadGuard{ std::move(lock), m_data };
    }

    [[nodiscard]]
    auto try_read() -> LockResult<ReadGuard> {
        typename ReadGuard::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) {
            return Err(Error(Code::ResourceLocked));
        }
        return Ok(ReadGuard{ std::move(lock), m_data });
    }

    [[nodiscard]]
    auto write() -> WriteGuard {
        typename WriteGuard::LockType lock{ m_mutex }; // blocking
        return WriteGuard{ std::move(lock), m_data };
    }

    [[nodiscard]]
    auto try_write() -> LockResult<WriteGuard> {
        typename WriteGuard::LockType lock{ m_mutex, std::try_to_lock };
        if (!lock.owns_lock()) {
            return Err(Error(Code::ResourceLocked));
        }
        return Ok(WriteGuard{ std::move(lock), m_data });
    }

private:
    T m_data;
    mutable std::shared_mutex m_mutex;
};

} // namespace siren::core
