#pragma once

#include "renderer/render_resource.hpp"
#include "utilities/spch.hpp"
#include "assets/asset.hpp"


namespace siren::core
{

class Image;
using ImageHandle = RenderResourceID<Image>;

/**
 * @brief Defines the amount of dimensions an Image may have.
 */
enum class ImageDimension { D1, D2, D3, Cube };

/**
 * @brief Defines the size of an image.
 */
struct ImageExtent {
    u32 width, height, depth_or_layers;
};

/**
 * @brief The format of the image. Manages both the CPU layout and
 * how the texture is stored in VRAM.
 */
enum class ImageFormat {
    Unknown,      ///< @brief Invalid format
    Mask8,        ///< @brief 1-Channel byte data.
    LinearColor8, ///< @brief 4-Channel (RGBA) byte data. (linear encoding)
    Color8,       ///< @brief 4-Channel (sRGBA) byte data. (sRGB encoding)
    Hdr16,        ///< @brief 3-Channel (RGB) HDR float data.
    DepthStencil, ///< @brief Depth Stencil Buffer data
};

/**
 * @brief Describes an @ref Image for creation.
 */
struct ImageDescriptor {
    std::optional<std::string> label; ///< @brief An optional label. Mainly used for debugging.
    std::span<const u8> img_data;     ///< @brief The actual image data. @todo do we want this here? staging buf?
    ImageFormat format;               ///< @brief The format of the image data (num channels/bytes per channel).
    ImageExtent extent;               ///< @brief Size of the image.
    ImageDimension dimension;         ///< @brief The dimensionality of the image.
    u32 mipmap_levels;                ///< @brief How many mip map levels to generate.
};

/**
 * @brief A gpu resource representing image data.
 */
class Image final : public RenderResource<Image> {
    using Base = RenderResource<Image>;

public:
    Image(
        Device* device,
        ImageHandle handle,
        const ImageFormat& image_format,
        const ImageExtent& image_extent,
        const ImageDimension& image_dimension,
        u32 mipmap_levels
    );

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    auto format() const -> ImageFormat { return m_format; }
    auto extent() const -> ImageExtent { return m_extent; }
    auto dimension() const -> ImageDimension { return m_dimension; }
    auto mipmap_levels() const -> u32 { return m_mipmap_levels; }

private:
    ImageFormat m_format;
    ImageExtent m_extent;
    ImageDimension m_dimension;
    u32 m_mipmap_levels;
};

/**
 * @brief An asset holding an Image and an ImageSampler.
 */
struct Texture : Asset {
    std::string name; ///< @brief The name of the Texture.
    Image image;      ///< @brief The underlying Image of the Texture.
    Sampler sampler;  ///< @brief The underlying ImageSampler of the Texture.

    Texture(const std::string& name, Image&& image, Sampler&& sampler)
        : name(name), image(std::move(image)), sampler(std::move(sampler)) { }
};
} // namespace siren::core
