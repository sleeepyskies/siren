#pragma once

#include "utilities/UUID.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

using AssetHandle = utilities::UUID;

enum class AssetType {
    NONE,
    MATERIAL,
    MESH,
    SHADER,
    TEXTURE2D,
    // SCENE,
    // STATIC_MESH,
    // SCRIPT,
    // AUDIO,
};

#define ASSET_TYPE(type)                                                                           \
    core::AssetType getAssetType() const override                                                  \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static core::AssetType getStaticAssetType()                                                    \
    {                                                                                              \
        return type;                                                                               \
    }

/**
 * @brief A base class for that all asset types should inherit from. Makes the asset usable by the
 * @ref AssetManager.
 */
class Asset
{
public:
    explicit Asset(const std::string& name) : m_name(name) {}
    virtual ~Asset() = default;

    virtual AssetType getAssetType() const = 0;
    const std::string& getName() const { return m_name; }

    // todo:
    // void serialize();
    // void deserialize();

private:
    std::string m_name{};
};

} // namespace siren::core

// make assets format-able by returning their name and thus usable by slog
template <>
struct std::formatter<siren::core::Asset> : std::formatter<std::string> {
    auto format(const siren::core::Asset& a, format_context& ctx) const
    {
        return formatter<std::string>::format(std::format("{}", a.getName()), ctx);
    }
};
template <>
struct std::formatter<std::shared_ptr<siren::core::Asset>> : std::formatter<std::string> {
    auto format(const std::shared_ptr<siren::core::Asset>& a, format_context& ctx) const
    {
        return formatter<std::string>::format(a ? a->getName() : "<null>", ctx);
    }
};
