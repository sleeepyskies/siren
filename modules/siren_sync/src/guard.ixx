module;

#include <type_traits>
#include <shared_mutex>

export module siren.sync:guard;

namespace siren::sync {

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
export template <typename T, typename Lock>
class Guard {
public:
    static constexpr bool IsReadonly = is_shared_lock<Lock>::value;

    using PointerType   = std::conditional_t<IsReadonly, const T*, T*>;
    using ReferenceType = std::conditional_t<IsReadonly, const T&, T&>;
    using LockType      = Lock;

    explicit Guard(
        Lock&& lock,
        ReferenceType data
    ) : m_lock(std::move(lock)), m_data(data) { }

    Guard(const Guard&)            = delete;
    Guard(Guard&&)                 = default;
    Guard& operator=(const Guard&) = delete;
    Guard& operator=(Guard&&)      = default;

    [[nodiscard]] constexpr auto operator->() noexcept -> PointerType { return &m_data; }
    [[nodiscard]] constexpr auto operator->() const noexcept -> const T* { return &m_data; }
    [[nodiscard]] constexpr auto operator*() noexcept -> ReferenceType { return m_data; }
    [[nodiscard]] constexpr auto operator*() const noexcept -> const T& { return m_data; }

private:
    friend class ConditionVariable;
    LockType m_lock;      ///< @brief The lock on the data.
    ReferenceType m_data; ///< @brief Reference to the data.
};

/**
 * @brief Represents unique temporary ownership of some resource for both
 * reading and writing.
 * @note The guard type returned by a @ref Mutex.
 */
export template <typename T>
using UniqueGuard = Guard<T, std::unique_lock<std::mutex>>;

/**
 * @brief Represents shared temporary ownership of some resource, but only
 * for reading rights.
 * @note The guard type returned by a @ref RwLock for reading.
 */
export template <typename T>
using ReadGuard = Guard<T, std::shared_lock<std::shared_mutex>>;

/**
 * @brief Represents unique temporary ownership of some resource, for
 * reading and writing rights.
 * @note The guard type returned by a @ref RwLock for writing.
 */
export template <typename T>
using WriteGuard = Guard<T, std::unique_lock<std::shared_mutex>>;

} // namespace siren::sync
