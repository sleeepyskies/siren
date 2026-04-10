export module siren.asset:asset_id;

import :asset;

import siren.common;
import siren.reflect;

namespace siren::asset {

/**
 * @brief Identifier for assets. Contains additional data such as a unique id, an index,
 * as well as type information.
 */
export struct AssetID final : Identifier64<AssetID> {
    using Identifier64::Identifier64;

    using GenerationType = GenerationType;
    using IndexType      = IndexType;
    using TypeID         = Meta;

    /**
     * @brief Constructs a new AssetID.
     * @param index The value to use for indexing this identifier.
     * @param gen The generation of this identifier.
     * @param type The type of this identifier.
     */
    AssetID(
        const IndexType index,
        const GenerationType gen,
        const TypeID type
    ) : Identifier64(index, gen, type) { }

    /** @brief Returns a 16-bit TypeID for the Asset of type A. */
    template <IsAsset A>
    [[nodiscard]]
    static constexpr TypeID get_type_id() noexcept { return static_cast<TypeID>(TypeHash<A>::hash()); }

    /** @brief Returns the TypeID of this AssetID. */
    [[nodiscard]] constexpr auto type() const noexcept -> TypeID { return m_meta; }
};

} // namespace siren::asset
