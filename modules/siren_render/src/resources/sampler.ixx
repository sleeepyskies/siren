module;

#include <optional>

export module siren.render.sampler;

import siren.common;
import siren.math;
import siren.render.render_resource;
import siren.render.device;

namespace siren::render {

/** @brief Tells the gpu how to filter the image. */
export enum class ImageFilterMode {
    /** @brief No filtering specified. */
    None,
    /** @brief Takes the value of the nearest neighbor. */
    Nearest,
    /** @brief Bilinear filtering. Interpolates color from neighboring pixels. */
    Linear,
};

/** @brief Tells the gpu how to handle coordinated outside the range [0, 1]. */
export enum class ImageWrapMode {
    /** @brief Repeats the image. */
    Repeat,
    /** @brief Mirrors the image. */
    Mirror,
    /** @brief Clamps the image color to the image border color. */
    ClampEdge,
    /** @brief Coordinates outside the range are given a user defined color. */
    ClampBorder,
};

/** @brief Tells the gpu how to compare depth values when sampling depth textures. */
export enum class ImageCompareMode {
    /** @brief Sample raw depth. */
    None,
    /** @brief Result may be either 0 or 1, uses ImageCompareFn. */
    Compare,
};

/**
 * @brief The comparison function to be used when sampling and comparing depth values.
 *
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glSamplerParameter.xhtml
 */
export enum class ImageCompareFn {
    Always,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
};

/** @brief Describes the ImageSampler for creation. */
export struct SamplerDescriptor {
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

/** @brief A gpu resource defining how to read from an Image. */
class Sampler : public RenderResource<Sampler> {
    using Base = RenderResource<Sampler>;

public:
    explicit Sampler(
        Device* device,
        Handle handle
    );
    ~Sampler();

    Sampler(Sampler&& other) noexcept;
    Sampler& operator=(Sampler&& other) noexcept;

    /** @brief Returns the Sampler's descriptor used to create it. */
    [[nodiscard]] auto descriptor() const noexcept -> const SamplerDescriptor&;
};

Sampler::Sampler(
    Device* device,
    const Handle handle
) : Base(device, handle) { }

Sampler::~Sampler() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_sampler(m_handle);
    }
}

Sampler::Sampler(Sampler&& other) noexcept
    : Base(std::move(other)) { }

Sampler& Sampler::operator=(Sampler&& other) noexcept {
    if (this != &other) {
        // cleanup old sampler
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_sampler(m_handle);
        }

        Base::operator=(std::move(other));
    }
    return *this;
}

auto Sampler::descriptor() const noexcept -> const SamplerDescriptor& { return m_device->sampler_descriptor(m_handle); }

} // namespace siren::render
