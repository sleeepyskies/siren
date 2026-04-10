module;

#include <optional>
#include <string>
#include <vector>

export module siren.render:framebuffer;

import :render_resource;
import :device;
import :image;

import siren.common;

namespace siren::render {

/** @brief Describes a @ref Framebuffer. Used for object creation via @ref Device. */
export struct FramebufferDescriptor {
    /** @brief The optional label of the @ref Framebuffer. Mainly used for debugging. */
    std::optional<std::string> label;
    /** @brief The width of the desired @ref Framebuffer in pixels. */
    u32 width;
    /** @brief The height of the desired @ref Framebuffer in pixels. */
    u32 height;
    /** @brief The number of color attachments. */
    u32 num_colors;
    /** @brief Whether there's number of color attachments. */
    bool has_depth_stencil;
};

/**
 * @class Framebuffer
 * @brief Represents a GPU framebuffer object along with its associated attachments.
 * Is essentially a collection of @ref Image's for color, depth and stencil attachments.
 * @todo store images in the backend instead?
 */
export class Framebuffer final : RenderResource<Framebuffer> {
    using Base = RenderResource<Framebuffer>;

public:
    explicit Framebuffer(
        Device* device,
        Handle handle,
        std::vector<Image>&& colors,
        std::optional<Image>&& depth_stencil
    );
    ~Framebuffer();

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    /** @brief Returns the @ref FramebufferDescriptor used to create this Framebuffer. */
    [[nodiscard]] auto descriptor() const noexcept -> const FramebufferDescriptor&;

    /** @brief Returns the color attachment of this Framebuffer. */
    [[nodiscard]] auto color_attachment(usize index) const noexcept -> const Image*;
    /** @brief Returns the depth_stencil attachment of this Framebuffer. */
    [[nodiscard]] auto depth_stencil_attachment() const noexcept -> const Image*;

private:
    /** @brief List of all color attachments. */
    std::vector<Image> m_colors;
    /** @brief The optional depth_stencil attachment. */
    std::optional<Image> m_depth_stencil;
};

Framebuffer::Framebuffer(
    Device* device,
    const Handle handle,
    std::vector<Image>&& colors,
    std::optional<Image>&& depth_stencil
) : Base(device, handle),
    m_colors(std::move(colors)),
    m_depth_stencil(std::move(depth_stencil)) { }

Framebuffer::~Framebuffer() {
    if (m_device&& m_handle
    .
    is_valid()
    )
    {
        m_device->destroy_framebuffer(m_handle);
    }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : Base(std::move(other)),
      m_colors(std::move(other.m_colors)) { }

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device&& m_handle
        .
        is_valid()
        )
        {
            m_device->destroy_framebuffer(m_handle);
        }

        Base::operator=(std::move(other));

        m_colors        = std::move(other.m_colors);
        m_depth_stencil = std::move(other.m_depth_stencil);
    }
    return *this;
}

auto Framebuffer::descriptor() const noexcept -> const FramebufferDescriptor& {
    return m_device->framebuffer_descriptor(m_handle);
}

auto Framebuffer::color_attachment(const usize index) const noexcept -> const Image* {
    if (index > m_colors.size()) { return nullptr; }
    return &m_colors[index];
}

auto Framebuffer::depth_stencil_attachment() const noexcept -> const Image* {
    if (m_depth_stencil.has_value()) { return &m_depth_stencil.value(); }
    return nullptr;
}

} // namespace siren::render
