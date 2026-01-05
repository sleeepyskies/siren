#pragma once

#include "utilities/UUID.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
using AssetHandle = utilities::UUID;

enum class AssetType
{
    None,
    Material,
    Mesh,
    // Shader,
    Texture2D,
    TextureCubeMap,
    GraphicsPipeline,
    // SCENE,
    // STATIC_MESH,
    // SCRIPT,
    // AUDIO,
};

#define ASSET_TYPE(type)                                                                           \
    core::AssetType GetAssetType() const override                                                  \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static core::AssetType GetStaticAssetType()                                                    \
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
    explicit Asset(const std::string& name) : m_name(name) { }

    virtual ~Asset() = default;

    virtual AssetType GetAssetType() const = 0;

    const std::string& GetName() const { return m_name; }

    // todo:
    // void serialize();
    // void deserialize();

private:
    std::string m_name{ };
};
} // namespace siren::core

// make assets format-able by returning their name and thus usable by slog
template <>
struct std::formatter<siren::core::Asset> : std::formatter<std::string>
{
    auto format(const siren::core::Asset& a, format_context& ctx) const
    {
        return formatter<std::string>::format(std::format("{}", a.GetName()), ctx);
    }
};

template <>
struct std::formatter<std::shared_ptr<siren::core::Asset>> : std::formatter<std::string>
{
    auto format(const std::shared_ptr<siren::core::Asset>& a, format_context& ctx) const
    {
        return formatter<std::string>::format(a ? a->GetName() : "<null>", ctx);
    }
};
