#pragma once

#include "core/identifier_64.hpp"


namespace siren::core
{

struct Asset;

/**
 * @brief A unique identifier for an asset. Wrapper around a 64-bit unsigned integer, split
 * into an index (32-bit) (used for AssetPool), a generation (16-bit), as well a type hash (16-bit).
 */
struct AssetID final : Identifier64<AssetID> {
    using Identifier64::Identifier64;

    using GenerationType = Identifier64::GenerationType;
    using IndexType      = Identifier64::IndexType;
    using TypeID         = Identifier64::Meta;

    AssetID(
        const IndexType idx,
        const GenerationType gen,
        const TypeID type
    ) : Identifier64(idx, gen, type) { }

    /// @brief Returns a 16-bit TypeID for the Asset of type A.
    template <typename A>
        requires(std::derived_from<A, Asset>)
    [[nodiscard]]
    static constexpr TypeID get_type_id() noexcept { return static_cast<TypeID>(TypeHash<A>::value().value()); }

    /// @brief Returns the TypeID of this AssetID.
    [[nodiscard]] constexpr auto type() const noexcept -> TypeID { return m_meta; }
};

} // namespace siren::core

