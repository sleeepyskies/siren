#pragma once

#include "sampler.hpp"

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
    /// @brief The desired width of the Image.
    u32 width;
    /// @brief The desired height of the Image.
    u32 height;
    /// @brief The desired amount of layers (for Image arrays) of depth (for 3D images) of the Image.
    u32 depth_or_layers;
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
        const ImageDescriptor& descriptor
    );
    ~Image();

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    /// @brief Returns the descriptor of this Image.
    [[nodiscard]] auto descriptor() const noexcept -> const ImageDescriptor&;

    /// @brief Upload data to this Image.
    auto upload(std::span<const u8> data) const noexcept -> std::expected<void, Error>;

private:
    ImageDescriptor m_descriptor;
};

} // namespace siren::core
