#include "framebuffer.hpp"

#include "renderer/device.hpp"
#include "utilities/spch.hpp"
#include <glad/glad.h>


namespace siren::core
{
Framebuffer::Framebuffer(
    Device* device,
    const FramebufferHandle handle,
    const FramebufferDescriptor& descriptor
) : Base(device, handle), m_descriptor(descriptor) { create(); }

Framebuffer::~Framebuffer() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_framebuffer(m_handle);
    }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : Base(std::move(other)), m_descriptor(std::move(other.m_descriptor)) { }

Framebuffer& Framebuffer::operator=(Framebuffer& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_framebuffer(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);
    }
    return *this;
}

auto Framebuffer::descriptor() const noexcept -> const FramebufferDescriptor& { return m_descriptor; }

void Framebuffer::set_viewport() const {
    glViewport(0, 0, descriptor.width, descriptor.height);
}

Texture* Framebuffer::color_attachment() const { return m_color.get(); }

Texture* Framebuffer::depth_attachment() const { return m_depth.get(); }

Texture* Framebuffer::stencil_attachment() const { return m_stencil.get(); }

void Framebuffer::resize(const u32 width, const u32 height) {
    if (m_handle.is_valid()) {
        glDeleteFramebuffers(1, &m_handle.value);
    }

    // update properties
    descriptor.width  = width;
    descriptor.height = height;

    // invalidate old data
    if (m_color) m_color.reset();
    if (m_depth) m_depth.reset();
    if (m_stencil) m_stencil.reset();

    // regenerate
    create();
    Logger::renderer->debug("Framebuffer resized to: ({}, {})", descriptor.width, descriptor.height);
}

void Framebuffer::create() {
    glCreateFramebuffers(1, &m_handle.value);

    // create attachments

    if (descriptor.has_color_buffer) {
        m_color = std::make_unique<Texture>(
            "Color Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::LinearColor8,
                ImageExtent{ descriptor.width, descriptor.height, 1 },
                ImageDimension::D2,
                0
            },
            Sampler{ { } }
        );
        glNamedFramebufferTexture(m_handle.value, GL_COLOR_ATTACHMENT0, m_color->image.handle().value, 0);
    }

    if (descriptor.has_depth_buffer) {
        m_depth = std::make_unique<Texture>(
            "Depth Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::DepthStencil,
                ImageExtent{ descriptor.width, descriptor.height, 1 },
                ImageDimension::D2,
                0
            },
            Sampler{ { } }
        );
        glNamedFramebufferTexture(m_handle.value, GL_DEPTH_ATTACHMENT, m_depth->image.handle().value, 0);
    }

    if (descriptor.has_stencil_buffer) {
        m_stencil = std::make_unique<Texture>(
            "Stencil Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::DepthStencil,
                ImageExtent{ descriptor.width, descriptor.height, 1 },
                ImageDimension::D2,
                0
            },
            Sampler{ { } }
        );
        glNamedFramebufferTexture(m_handle.value, GL_STENCIL_ATTACHMENT, m_stencil->image.handle().value, 0);
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SIREN_ASSERT(false, "FrameBuffer could not be created");
    }
}
} // namespace siren::core
