module;

#include <optional>
#include <string>

export module siren.render:image;

import :render_resource;
import :device;
import :sampler;

import siren.common;

namespace siren::render {

/** @brief Defines the amount of dimensions an Image may have. */
export enum class ImageDimension {
    D1, D2, D3, Cube
};

/** @brief Defines the size of an image. */
export struct ImageExtent {
    /** @brief The desired width of the Image. */
    u32 width;
    /** @brief The desired height of the Image. */
    u32 height;
    /** @brief The desired amount of layers (for Image arrays) of depth (for 3D images) of the Image. */
    u32 depth_or_layers;
};

/**
 * @brief The format of the image. Manages both the CPU layout and
 * how the texture is stored in VRAM.
 */
export enum class ImageFormat {
    /** @brief Invalid format */
    Unknown,
    /** @brief 1-Channel byte data. */
    Mask8,
    /** @brief 4-Channel (RGBA) byte data. (linear encoding) */
    LinearColor8,
    /** @brief 4-Channel (sRGBA) byte data. (sRGB encoding) */
    Color8,
    /** @brief 3-Channel (RGB) HDR float data. */
    Hdr16,
    /** @brief Depth Stencil Buffer data */
    DepthStencil,
};

/** @brief Describes an @ref Image for creation. */
export struct ImageDescriptor {
    /** @brief An optional label. Mainly used for debugging. */
    std::optional<std::string> label;
    /** @brief The format of the image data (num channels/bytes per channel). */
    ImageFormat format;
    /** @brief Size of the image. */
    ImageExtent extent;
    /** @brief The dimensionality of the image. */
    ImageDimension dimension;
    /** @brief How many mip map levels to generate. */
    u32 mipmap_levels;
};

/** @brief A gpu resource representing image data. */
export class Image final : public RenderResource<Image> {
    using Base = RenderResource<Image>;

public:
    Image(
        Device* device,
        Handle handle
    );
    ~Image();

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    /** @brief Returns the descriptor of this Image. */
    [[nodiscard]] auto descriptor() const noexcept -> const ImageDescriptor&;
};

Image::Image(
    Device* device,
    const Handle handle
) : Base(device, handle) { }

Image::~Image() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_image(m_handle);
    }
}

Image::Image(Image&& other) noexcept
    : Base(std::move(other)) { }

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_image(m_handle);
        }

        Base::operator=(std::move(other));
    }
    return *this;
}

auto Image::descriptor() const noexcept -> const ImageDescriptor& { return m_device->image_descriptor(m_handle); }

} // namespace siren::render
