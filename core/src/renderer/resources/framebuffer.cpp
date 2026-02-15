#include "framebuffer.hpp"

#include "renderer/device.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
Framebuffer::Framebuffer(
    Device* device,
    const FramebufferHandle handle,
    const FramebufferDescriptor& descriptor,
    std::optional<Image>&& color,
    std::optional<Image>&& depth,
    std::optional<Image>&& stencil
) : Base(device, handle),
    m_descriptor(descriptor),
    m_color(std::move(color)),
    m_depth(std::move(depth)),
    m_stencil(std::move(stencil)) { }

Framebuffer::~Framebuffer() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_framebuffer(m_handle);
    }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : Base(std::move(other)),
      m_descriptor(std::move(other.m_descriptor)),
      m_color(std::move(other.m_color)),
      m_depth(std::move(other.m_depth)),
      m_stencil(std::move(other.m_stencil)) { }

Framebuffer& Framebuffer::operator=(Framebuffer& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_framebuffer(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);

        m_color   = std::move(other.m_color);
        m_depth   = std::move(other.m_depth);
        m_stencil = std::move(other.m_stencil);
    }
    return *this;
}

auto Framebuffer::descriptor() const noexcept -> const FramebufferDescriptor& { return m_descriptor; }

auto Framebuffer::color_attachment() const noexcept -> const Image* {
    if (m_color.has_value()) { return &m_color.value(); }
    return nullptr;
}

auto Framebuffer::depth_attachment() const noexcept -> const Image* {
    if (m_depth.has_value()) { return &m_depth.value(); }
    return nullptr;
}

auto Framebuffer::stencil_attachment() const noexcept -> const Image* {
    if (m_stencil.has_value()) { return &m_stencil.value(); }
    return nullptr;
}

} // namespace siren::core
