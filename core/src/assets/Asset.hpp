#pragma once

#include "utilities/UUID.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{

using AssetHandle = utilities::UUID;

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
