#pragma once

#include "renderer/render_resource.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

class Sampler;
using SamplerHandle = RenderResourceID<Sampler>;

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
struct SamplerDescriptor {
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
class Sampler : public RenderResource<Sampler> {
    using Base = RenderResource<Sampler>;

public:
    explicit Sampler(
        Device* device,
        SamplerHandle handle,
        const SamplerDescriptor& descriptor
    );
    ~Sampler();

    Sampler(Sampler&& other) noexcept;
    Sampler& operator=(Sampler&& other) noexcept;

    /// @brief Returns the Sampler's descriptor used to create it.
    [[nodiscard]] auto descriptor() const noexcept -> const SamplerDescriptor& { return m_descriptor; }

private:
    SamplerDescriptor m_descriptor;
};

} // namespace siren::core
