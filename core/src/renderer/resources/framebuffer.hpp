#pragma once

#include "image.hpp"
#include "renderer/render_resource.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
// todo: can we optimise here using a render buffer object?

class Image;

class Framebuffer;
using FramebufferHandle = RenderResourceID<Framebuffer>;

/**
 * @brief Describes a @ref Framebuffer. Used for object creation via @ref Device.
 */
struct FramebufferDescriptor {
    /// @brief The optional label of the @ref Framebuffer. Mainly used for debugging.
    std::optional<std::string> label;
    /// @brief The width of the desired @ref Framebuffer in pixels.
    u32 width;
    /// @brief The height of the desired @ref Framebuffer in pixels.
    u32 height;
    u32 num_samples = 1; // TODO: not used for now :D
    bool has_color;
    bool has_depth;
    bool has_stencil;
};

class Framebuffer final : RenderResource<Framebuffer> {
    using Base = RenderResource<Framebuffer>;

public:
    explicit Framebuffer(
        Device* device,
        FramebufferHandle handle,
        const FramebufferDescriptor& descriptor,
        std::optional<Image>&& color,
        std::optional<Image>&& depth,
        std::optional<Image>&& stencil
    );
    ~Framebuffer();

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer& other) noexcept;

    /// @brief Returns the @ref FramebufferDescriptor used to create this Framebuffer.
    [[nodiscard]] auto descriptor() const noexcept -> const FramebufferDescriptor&;

    // todo: resolve the handle to an Image* right here?

    /// @brief Returns the color attachment of this Framebuffer.
    [[nodiscard]] auto color_attachment() const noexcept -> const Image*;
    /// @brief Returns the depth attachment of this Framebuffer.
    [[nodiscard]] auto depth_attachment() const noexcept -> const Image*;
    /// @brief Returns the stencil attachment of this Framebuffer.
    [[nodiscard]] auto stencil_attachment() const noexcept -> const Image*;

    auto resize(u32 width, u32 height) -> void;

private:
    /// @brief The construction parameters of the Framebuffer.
    FramebufferDescriptor m_descriptor;

    /// @brief The optional color attachment.
    std::optional<Image> m_color;
    /// @brief The optional depth attachment.
    std::optional<Image> m_depth;
    /// @brief The optional stencil attachment.
    std::optional<Image> m_stencil;
};
} // namespace siren::core
