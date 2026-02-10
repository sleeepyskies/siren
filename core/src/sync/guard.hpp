#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

template <typename L>
struct is_shared_lock : std::false_type { };

template <typename M>
struct is_shared_lock<std::shared_lock<M>> : std::true_type { };

/**
 * @class Guard
 * @brief A RAII container for accessing shared data in a thread safe manner.
 * The lock/guard is held until this object is dropped.
 * @todo Guard poisoning?
 * @tparam T The type of the data behind the guard.
 * @tparam Lock The type of the lock. Maybe either a shared or a unique lock.
 */
template <typename T, typename Lock>
class Guard {
public:
    static constexpr bool IsReadonly = is_shared_lock<Lock>::value;

    using Resource  = T;
    using Pointer   = std::conditional_t<IsReadonly, const T*, T*>;
    using Reference = std::conditional_t<IsReadonly, const T&, T&>;
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
    friend class ConditionVariable;
    LockType m_lock;  ///< @brief The lock on the data.
    Reference m_data; ///< @brief Reference to the data.
};

} // namespace siren::core
