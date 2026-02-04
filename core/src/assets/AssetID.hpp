#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @brief A unique identifier for an asset. Wrapper around a 64-bit unsigned integer, split
 * into an index (32-bit) (used for AssetPool), a generation (16-bit), as well a type hash (16-bit).
 */
struct AssetID {
    using Index      = u32;
    using Generation = u16;
    using TypeID     = u16;

    union {
        u64 id = 0; ///< @brief The full packed ID.
        struct {
            u64 type       : 16; ///< @brief The type of the asset
            u64 generation : 16; ///< @brief The generation of this id.
            u64 index      : 32; ///< @brief The value used for indexing.
        };
    };

    AssetID() = default;
    AssetID(
        const Index idx,
        const Generation gen,
        const TypeID type
    ) : type(type), generation(gen), index(idx) { }

    AssetID(const AssetID&)            = default;
    AssetID& operator=(const AssetID&) = default;
    AssetID(AssetID&&)                 = default;
    AssetID& operator=(AssetID&&)      = default;

    [[nodiscard]]
    bool is_valid() const { return id != 0; }
    [[nodiscard]]
    explicit operator bool() const { return is_valid(); }

    [[nodiscard]]
    bool operator==(const AssetID& other) const { return id == other.id; }
    [[nodiscard]]
    auto operator<=>(const AssetID& other) const { return id <=> other.id; }

    /// @brief Returns a 16-bit type id for the type A.
    template <typename A>
    [[nodiscard]]
    static constexpr TypeID get_type_id() noexcept { return static_cast<TypeID>(TypeHash<A>::value().value()); }
};

} // namespace siren::core
template <>
struct std::hash<siren::core::AssetID> {
    size_t operator()(const siren::core::AssetID& id) const noexcept { return id.id; }
};
