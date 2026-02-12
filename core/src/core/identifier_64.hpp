#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @struct Identifier64
 * @brief A packed 64-bit integer handle. Provides unique identification
 * and can prevent stale references. Also allows for additional meta-data
 * for the use.
 * @tparam T The derived CRTP type.
 * @note Bit layout (MSB to LSB):
 * - [32-63] Index: Lookup key.
 * - [16-31] Generation: Should be incremented each time a slot is reused to avoid invalid references.
 * - [00-15] Meta: Some user defined extra meta-data.
 */
template <typename T>
class Identifier64 {
protected:
    /// @brief The main storage type of the packed id.
    using IdType = u64;
    /// @brief The type used for indexing.
    using IndexType = u32;
    /// @brief The type used for the generation of the id.
    using GenerationType = u16;
    /// @brief The type used for custom additional data.
    using Meta = u16;

    union {
        IdType m_id = 0; ///< @brief The full packed ID.
        struct {
            IdType m_meta       : 16; ///< @brief Extra meta-data to be specified by user.
            IdType m_generation : 16; ///< @brief The generation of this id.
            IdType m_index      : 32; ///< @brief The value used for indexing.
        };
    };

    Identifier64() = default;
    Identifier64(
        const IndexType idx,
        const GenerationType gen,
        const Meta meta
    ) : m_meta(meta), m_generation(gen), m_index(idx) { }

public:
    Identifier64(const Identifier64&)            = default;
    Identifier64& operator=(const Identifier64&) = default;
    Identifier64(Identifier64&&)                 = default;
    Identifier64& operator=(Identifier64&&)      = default;

    /// @brief Returns the full packed value of this id.
    [[nodiscard]]
    constexpr auto packed() const noexcept -> IdType { return m_id; }

    /// @brief Returns the index of this id.
    [[nodiscard]]
    constexpr auto index() const noexcept -> IndexType { return m_index; }

    /// @brief Returns the generation of this id.
    [[nodiscard]]
    constexpr auto generation() const noexcept -> GenerationType { return m_generation; }

    /// @brief Simple factory method to return an invalid Identifier64.
    [[nodiscard]]
    constexpr static auto invalid() noexcept -> Identifier64 { return { 0, 0, 0 }; }

    /// @brief Checks if the handle is valid aka has a non 0 inner value.
    [[nodiscard]]
    constexpr auto is_valid() const noexcept -> bool { return m_id != 0; }

    /// @brief Kills the handle by zeroing its value.
    constexpr auto invalidate() noexcept -> void { m_id = 0; }

    /// @copydoc is_valid
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept { return is_valid(); }

    /// @brief Equality comparison based on the inner 64-bit value.
    [[nodiscard]]
    auto operator==(const Identifier64& other) const -> bool { return packed() == other.packed(); }
    /// @brief Inequality comparison based on the inner 64-bit value.
    [[nodiscard]]
    auto operator<=>(const Identifier64& other) const { return packed() <=> other.packed(); }
};

} // namespace siren::core

template <typename T>
struct std::hash<siren::core::Identifier64<T>> {
    auto operator()(const siren::core::Identifier64<T>& identifier) const noexcept -> size_t {
        return static_cast<size_t>(identifier.packed());
    }
};
