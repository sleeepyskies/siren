module;

#include <compare>
#include <functional>

export module siren.common:hashed_string;

import :typedefs;

// Magic numbers used for 64-bit FNV-1a hashing.
constexpr auto PRIME  = 1099511628211ull;
constexpr auto OFFSET = 14695981039346656037ull;


namespace siren
{

/**
 * @brief A class representing a hashed string.
 */
export class HashedString {
public:
    /**
     * @brief Creates a new hashes_string using the FNV-1a hash algorithm.
     * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
     * @param string The null terminated string to hash.
     * @return A 64-bit hash of the input string.
     */
    constexpr HashedString(const char* string) noexcept : m_hash(OFFSET), m_name(string), m_length(0) {
        if (!string) {
            m_hash = 0;
            return;
        }
        for (const char* c = string; *c; c++) {
            m_hash ^= *c;
            m_hash *= PRIME;
            m_length++;
        }
    }

    constexpr HashedString() noexcept : m_hash(0), m_name(nullptr), m_length(0) { }

    HashedString(const HashedString&)            = default;
    HashedString(HashedString&&)                 = default;
    HashedString& operator=(const HashedString&) = default;
    HashedString& operator=(HashedString&&)      = default;

    /// @brief Three-way comparison between two hashed_string's.
    [[nodiscard]]
    constexpr auto operator<=>(const HashedString& other) const noexcept {
        return m_hash <=> other.m_hash;
    }

    /// @brief Equality comparison.
    [[nodiscard]]
    constexpr auto operator==(const HashedString& other) const noexcept -> bool {
        return m_hash == other.m_hash;
    }

    /// @brief Returns the computed 64-bit hash.
    [[nodiscard]]
    constexpr auto value() const noexcept -> u64 { return m_hash; }

    /// @brief Returns the original string used to construct this hash.
    [[nodiscard]]
    constexpr auto data() const noexcept -> const char* { return m_name; }

    /// @brief Returns the length of the original string.
    [[nodiscard]]
    constexpr auto length() const noexcept -> size_t { return m_length; }

    [[nodiscard]]
    explicit operator bool() const noexcept { return m_hash != 0; }

    /// @brief Returns the underlying value of the HashedString.
    [[nodiscard]]
    constexpr auto hash() const noexcept { return value(); }

private:
    /// @brief The computed hash.
    u64 m_hash;
    /// @brief The original string.
    const char* m_name;
    /// @brief The length of the original string.
    usize m_length;
};

consteval HashedString operator ""_hs(const char* str) { return HashedString{ str }; }
} // namespace siren::core
