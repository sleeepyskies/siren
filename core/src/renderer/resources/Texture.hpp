#pragma once

#include "GpuResource.hpp"
#include "utilities/spch.hpp"
#include "assets/Asset.hpp"


namespace siren::core
{

class ImageSampler;
class Image;

/// @brief The ID type of the ImageSampler.
using ImageSamplerHandle = GpuResourceHandle<ImageSampler>;
/// @brief The ID type of the Image.
using ImageHandle = GpuResourceHandle<Image>;

/**
 * @brief Tells the gpu how to filter the image.
 */
enum class ImageFilterMode {
    None,    ///< @brief No filtering specified.
    Nearest, ///< @brief Takes the value of the nearest neighbor.
    Linear,  ///< @brief Bilinear filtering. Interpolates color from neighboring pixels.
};

/**
 * @brief Tells the gpu how to handle coordinated outside the range [0, 1].
 */
enum class ImageWrapMode {
    Repeat,      ///< @brief Repeats the image.
    Mirror,      ///< @brief Mirrors the image.
    ClampEdge,   ///< @brief Clamps the image color to the image border color.
    ClampBorder, ///< @brief Coordinates outside the range are given a user defined color.
};

/**
 * @brief Tells the gpu how to compare depth values when sampling depth textures.
 */
enum class ImageCompareMode {
    None,    ///< @brief Sample raw depth.
    Compare, ///< @brief Result may be either 0 or 1, uses ImageCompareFn.
};

/**
 * @brief The comparison function to be used when sampling and comparing depth values.
 *
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glSamplerParameter.xhtml
 */
enum class ImageCompareFn {
    Always,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
};

/**
 * @brief Describes the ImageSampler for creation.
 */
struct ImageSamplerDescription {
    /// @brief Tells the gpu how to filter when the source image is smaller.
    ImageFilterMode min_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to filter when the source image is larger.
    ImageFilterMode max_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to filter between mipmap levels.
    ImageFilterMode mipmap_filter = ImageFilterMode::Nearest;
    /// @brief Tells the gpu how to wrap along the horizontal axis.
    ImageWrapMode s_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu how to wrap along the vertical axis.
    ImageWrapMode t_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu how to wrap along the depth axis.
    ImageWrapMode r_wrap = ImageWrapMode::Repeat;
    /// @brief Tells the gpu the highest resolution mipmap it can use.
    f32 lod_min = 0.f;
    /// @brief Tells the gpu the lowest resolution mipmap it can use.
    f32 lod_max = 0.f;
    /// @brief A custom user specified color for the image border.
    std::optional<glm::vec4> border_color = std::nullopt;
    /// @brief Tells the gpu how to sample depth.
    ImageCompareMode compare_mode = ImageCompareMode::None;
    /// @brief The function with which to sample depth.
    ImageCompareFn compare_fn = ImageCompareFn::LessEqual;
};

/**
 * @brief A gpu resource defining how to read from an Image.
 */
class ImageSampler : public GpuResource {
public:
    explicit ImageSampler(ImageSamplerDescription&& desc);
    ~ImageSampler() override;

    ImageSampler(ImageSampler&& other) noexcept : m_handle(other.m_handle), m_desc(std::move(other.m_desc)) {
        other.m_handle = { 0 };
    }

    ImageSampler& operator=(ImageSampler&& other) noexcept;
    auto handle() const -> ImageSamplerHandle { return m_handle; }
    auto descriptor() const -> const ImageSamplerDescription& { return m_desc; }

private:
    ImageSamplerHandle m_handle;
    ImageSamplerDescription m_desc;
};

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
 * @brief A gpu resource representing image data.
 */
class Image final : public GpuResource {
public:
    Image(
        std::span<const u8> data,
        const ImageFormat& image_format,
        const ImageExtent& image_extent,
        const ImageDimension& image_dimension,
        u32 mipmap_levels
    );
    ~Image() override;

    Image(Image&& other) noexcept
        : m_handle(other.m_handle), m_format(other.m_format), m_extent(other.m_extent),
          m_dimension(other.m_dimension), m_mipmap_levels(other.m_mipmap_levels) { other.m_handle = { }; }
    Image& operator=(Image&& other) noexcept;

    auto handle() const -> ImageHandle { return m_handle; }
    auto format() const -> ImageFormat { return m_format; }
    auto extent() const -> ImageExtent { return m_extent; }
    auto dimension() const -> ImageDimension { return m_dimension; }
    auto mipmap_levels() const -> u32 { return m_mipmap_levels; }

private:
    ImageHandle m_handle;
    ImageFormat m_format;
    ImageExtent m_extent;
    ImageDimension m_dimension;
    u32 m_mipmap_levels;
};

/**
 * @brief An asset holding an Image and an ImageSampler.
 */
struct Texture : Asset {
    std::string name;     ///< @brief The name of the Texture.
    Image image;          ///< @brief The underlying Image of the Texture.
    ImageSampler sampler; ///< @brief The underlying ImageSampler of the Texture.

    Texture(const std::string& name, Image&& image, ImageSampler&& sampler)
        : name(name), image(std::move(image)), sampler(std::move(sampler)) { }
};
} // namespace siren::core
