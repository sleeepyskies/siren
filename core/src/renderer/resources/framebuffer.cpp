#include "framebuffer.hpp"

#include "utilities/spch.hpp"
#include <glad/glad.h>


namespace siren::core
{
Framebuffer::Framebuffer(const Description& description) : m_description(description) { create(); }

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_handle.value);
}

const Framebuffer::Description& Framebuffer::description() const { return m_description; }

void Framebuffer::bind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_handle.value);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

FramebufferHandle Framebuffer::handle() const { return m_handle; }

void Framebuffer::set_viewport() const {
    glViewport(0, 0, m_description.width, m_description.height);
}

Texture* Framebuffer::color_attachment() const { return m_color.get(); }

Texture* Framebuffer::depth_attachment() const { return m_depth.get(); }

Texture* Framebuffer::stencil_attachment() const { return m_stencil.get(); }

void Framebuffer::resize(const u32 width, const u32 height) {
    if (m_handle.is_valid()) {
        glDeleteFramebuffers(1, &m_handle.value);
    }

    // update properties
    m_description.width  = width;
    m_description.height = height;

    // invalidate old data
    if (m_color) m_color.reset();
    if (m_depth) m_depth.reset();
    if (m_stencil) m_stencil.reset();

    // regenerate
    create();
    Logger::renderer->debug("Framebuffer resized to: ({}, {})", m_description.width, m_description.height);
}

void Framebuffer::create() {
    // need to have at least one attachment
    SIREN_ASSERT(
        m_description.has_color_buffer || m_description.has_depth_buffer ||
        m_description.has_stencil_buffer,
        "FrameBuffer must be created with at least one buffer attachment"
    );

    glCreateFramebuffers(1, &m_handle.value);

    // create attachments

    if (m_description.has_color_buffer) {
        m_color = std::make_unique<Texture>(
            "Color Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::LinearColor8,
                ImageExtent{ m_description.width, m_description.height, 1 },
                ImageDimension::D2,
                0
            },
            Sampler{ { } }
        );
        glNamedFramebufferTexture(m_handle.value, GL_COLOR_ATTACHMENT0, m_color->image.handle().value, 0);
    }

    if (m_description.has_depth_buffer) {
        m_depth = std::make_unique<Texture>(
            "Depth Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::DepthStencil,
                ImageExtent{ m_description.width, m_description.height, 1 },
                ImageDimension::D2,
                0
            },
            Sampler{ { } }
        );
        glNamedFramebufferTexture(m_handle.value, GL_DEPTH_ATTACHMENT, m_depth->image.handle().value, 0);
    }

    if (m_description.has_stencil_buffer) {
        m_stencil = std::make_unique<Texture>(
            "Depth Attachment",
            Image{
                std::span<const u8>{ },
                ImageFormat::DepthStencil,
                ImageExtent{ m_description.width, m_description.height, 1 },
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
