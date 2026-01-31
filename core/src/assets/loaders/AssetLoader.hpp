/**
 * @file AssetLoader.hpp
 * @brief Home for the definitions of all asset loaders.
 */
#pragma once

#include "core/Result.hpp"
#include "renderer/resources/Texture.hpp"


namespace siren::core
{
class AssetServer;
class LoadContext;

/// @brief Provides loader configuration.
struct LoaderConfig {
    virtual ~LoaderConfig() = 0;
};

/// @brief Loader configuration for loading gltf files.
struct GltfLoaderConfig final : LoaderConfig {
    /// bool triangulate_meshes = true;
};

/// @brief Determines how to interpret texture data as a 2D TextureArray. Siren requires the
/// images are stacked on top of each other and all have the same dimensions.
struct ImageArrayLayout {
    u32 row_height; ///< @brief The height of each texture in the array.
};

/// @brief Loader configuration for loading texture files.
struct TextureLoaderConfig final : LoaderConfig {
    /// @brief The name of the Texture to load. Uses filename if not present.
    Option<String> name = Option<String>(None);
    /// @brief The format of the Texture to load. Guesses if not present.
    Option<ImageFormat> format = Option<ImageFormat>(None);
    /// @brief The sampler of the Texture to load.
    ImageSampler sampler{ ImageSamplerDescription{ } };
    /// @brief If present, determines how to interpret the array of textures.
    Option<ImageArrayLayout> array_layout = Option<ImageArrayLayout>(None);
    /// @brief Whether the image is in linear space or in sRGB space.
    bool is_srgb = false;
    /// @brief If true, generates 1 + floor(log2(max(w, h, d))) mipmap levels.
    bool generate_mipmap_levels = false;
};

/**
 * @brief Base class for standard AssetLoaders.
 */
struct AssetLoader {
    virtual ~AssetLoader() = default;
    virtual auto load(LoadContext&& ctx, const LoaderConfig* config) const -> Result<Unit, Error> = 0;
    constexpr virtual auto extensions() const -> std::span<const std::string_view> = 0;
};

class GltfLoader final : public AssetLoader {
public:
    auto load(LoadContext&& ctx, const LoaderConfig* config) const -> Result<Unit, Error> override;
    constexpr auto extensions() const -> std::span<const std::string_view> override { return m_exts; }
    constexpr auto default_config() const -> GltfLoaderConfig { return { }; }

private:
    static constexpr std::string_view m_exts[] = { "glb", "gltf" };
};

class TextureLoader final : public AssetLoader {
public:
    auto load(LoadContext&& ctx, const LoaderConfig* config) const -> Result<Unit, Error> override;
    constexpr auto extensions() const -> std::span<const std::string_view> override { return m_exts; }
    constexpr auto default_config() const -> TextureLoaderConfig { return { }; }

private:
    static constexpr std::string_view m_exts[] = { "png", "jpg", "jpeg" };
};
} // namespace siren::core
