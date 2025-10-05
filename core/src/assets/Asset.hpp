#pragma once

#include "utilities/spch.hpp"

namespace siren::assets
{

enum class AssetType {
    NONE,
    // TODO TEXTURE2D,
    // TODO MATERIAL,
    MESH,
    MODEL,
    SHADER,
    SCENE, // TODO: make scene
};

#define ASSET_TYPE(type)                                                                           \
    assets::AssetType getType() const override                                                     \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static assets::AssetType getStaticType()                                                       \
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
    /// @brief A virtual asset is a dependent resource without a standalone file.
    bool isVirtualAsset;
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

    /// @brief Constructs and returns an invalid AssetHandle that does not reference an asset
    static AssetHandle invalid();

    bool operator==(const AssetHandle&) const;
    bool operator<(const AssetHandle&) const; // usable in ordered data structures
    explicit operator bool() const;

private:
    /// @brief We use just a 64-bit handle as this is far more than enough for our use case.
    /// Around 607 million assets would need to be imported for a 0.1% chance of a duplicate uuid
    uint64_t m_uuid = 0;

    /// @brief Used only internally to construct an invalid AssetHandle. Is useful for default
    /// member variables that are overridden by constructor parameters to indicate a failure.
    explicit AssetHandle(const uint64_t uuid);

    friend struct std::hash<AssetHandle>; // allow access for hashing
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
    virtual ~Asset() = default;

    virtual AssetType getType() const = 0;
    const std::string& getName() const
    {
        return m_name;
    }

private:
    std::string m_name{};
};

} // namespace siren::assets

// make asset handle hashable and usable as a key in hash maps
template <>
struct std::hash<siren::assets::AssetHandle> {
    size_t operator()(const siren::assets::AssetHandle& handle) const noexcept
    {
        return ::std::hash<uint64_t>{}(handle.m_uuid);
    }
};

// make assets format-able by returning their name and thus usable by slog
template <>
struct std::formatter<siren::assets::Asset> : std::formatter<std::string> {
    auto format(const siren::assets::Asset& a, format_context& ctx) const
    {
        return formatter<std::string>::format(std::format("{}", a.getName()), ctx);
    }
};
template <>
struct std::formatter<std::shared_ptr<siren::assets::Asset>> : std::formatter<std::string> {
    auto format(const std::shared_ptr<siren::assets::Asset>& a, format_context& ctx) const
    {
        return formatter<std::string>::format(a ? a->getName() : "<null>", ctx);
    }
};
