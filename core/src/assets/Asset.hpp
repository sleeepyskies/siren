#pragma once

#include "utilities/spch.hpp"

namespace core::assets
{

enum class AssetType {
    NONE,
    TEXTURE2D,
    MATERIAL,
    MODEL,
    SHADER,
    SCENE, // TODO: make scene
};

#define ASSET_TYPE(type)                                                                           \
    assets::AssetType getAssetType() const override                                                \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static assets::AssetType getStaticAssetType()                                                  \
    {                                                                                              \
        return type;                                                                               \
    }

/**
 * @brief Defines useful information regarding any loaded assets.
 */
struct AssetMetaData {
    /// @brief The filepath of the asset relative to the assets/ directory
    fs::path filePath;
    /// @brief The type of the asset
    AssetType type;
};

/**
 * @brief A universally unique way to identify an asset in siren.
 */
class AssetHandle
{
public:
    /// @brief Assigns a random uuid on construction
    AssetHandle();
    AssetHandle(const AssetHandle&)            = default;
    AssetHandle& operator=(const AssetHandle&) = default;

    bool operator==(const AssetHandle&) const;
    bool operator<(const AssetHandle&) const; // usable in ordered data structures
    explicit operator bool() const;

    uint64_t uuid() const; // only provided to make AssetHandle hashable

private:
    /// @brief We use just a 64-bit handle as this is far more than enough for our use case.
    /// Around 607 million assets would need to be imported for a 0.1% chance of a duplicate uuid
    uint64_t m_uuid = 0;
};

/**
 * @brief A base class for that all asset types should inherit from. Makes the asset usable by the
 * @ref AssetManager.
 */
class Asset
{
public:
    explicit Asset(const std::string& name) : m_name(name)
    {
    }
    virtual ~Asset() = 0;

    virtual AssetType getAssetType() const = 0;

private:
    std::string m_name{};
};

} // namespace core::assets

template <>
struct std::hash<core::assets::AssetHandle> {
    size_t operator()(const core::assets::AssetHandle& handle) const noexcept
    {
        return ::std::hash<uint64_t>{}(handle.uuid());
    }
}; // namespace std
