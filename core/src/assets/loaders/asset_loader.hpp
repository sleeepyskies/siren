/**
 * @file AssetLoader.hpp
 * @brief Home for the definitions of all asset loaders.
 */
#pragma once

#include "utilities/spch.hpp"
#include "renderer/resources/texture.hpp"


namespace siren::core
{
class AssetServer;
class LoadContext;

/// @brief Loader configuration for loading gltf files.
struct GltfLoaderConfig {
    /// bool triangulate_meshes = true;
};

/// @brief Determines how to interpret texture data as a 2D TextureArray. Siren requires the
/// images are stacked on top of each other and all have the same dimensions.
struct ImageArrayLayout {
    u32 row_height; ///< @brief The height of each texture in the array.
};

/// @brief Loader configuration for loading texture files.
struct TextureLoaderConfig {
    /// @brief The name of the Texture to load. Uses filename if not present.
    std::optional<std::string> name = std::nullopt;
    /// @brief The format of the Texture to load. Guesses if not present.
    std::optional<ImageFormat> format = std::nullopt;
    /// @brief The sampler of the Texture to load.
    Sampler sampler{ SamplerDescriptor{ } };
    /// @brief If present, determines how to interpret the array of textures.
    std::optional<ImageArrayLayout> array_layout = std::nullopt;
    /// @brief Whether the image is in linear space or in sRGB space.
    bool is_srgb = false;
    /// @brief If true, generates 1 + floor(log2(max(w, h, d))) mipmap levels.
    bool generate_mipmap_levels = false;
};

struct ShaderLoaderConfig { };

/// @brief Provides loader configuration.
using LoaderConfig = std::variant<
    GltfLoaderConfig,
    TextureLoaderConfig
>;

/**
 * @brief Base class for standard AssetLoaders.
 *
 * See @ref Asset.
 */
struct AssetLoader {
    virtual ~AssetLoader() = default;
    virtual auto load(LoadContext&& ctx, const LoaderConfig& config) const -> std::expected<void, Error> = 0;
    constexpr virtual auto extensions() const -> std::span<const std::string_view> = 0;
};

/**
 * @brief Loader for gltf files. Loads gltf assets and all sub-assets.
 *
 * See @ref Gltf
 */
class GltfLoader final : public AssetLoader {
public:
    auto load(LoadContext&& ctx, const LoaderConfig& config) const -> std::expected<void, Error> override;
    constexpr auto extensions() const -> std::span<const std::string_view> override { return m_exts; }
    constexpr auto default_config() const -> GltfLoaderConfig { return { }; }

private:
    static constexpr std::string_view m_exts[] = { "glb", "gltf" };
};

/**
 * @brief Loader for images.
 *
 * See @ref Texture
 */
class TextureLoader final : public AssetLoader {
public:
    auto load(LoadContext&& ctx, const LoaderConfig& config) const -> std::expected<void, Error> override;
    constexpr auto extensions() const -> std::span<const std::string_view> override { return m_exts; }
    constexpr auto default_config() const -> TextureLoaderConfig { return TextureLoaderConfig{ }; }

private:
    static constexpr std::string_view m_exts[] = { "png", "jpg", "jpeg" };
};

/**
 * @brief Loader for shaders.
 *
 * See @ref Shader
 */
class ShaderLoader final : public AssetLoader {
public:
    auto load(LoadContext&& ctx, const LoaderConfig& config) const -> std::expected<void, Error> override;
    constexpr auto extensions() const -> std::span<const std::string_view> override { return m_exts; }
    constexpr auto default_config() const -> ShaderLoaderConfig { return ShaderLoaderConfig{ }; }

private:
    static constexpr std::string_view m_exts[] = { "sshg" };
};
} // namespace siren::core
