#include "FrameBuffer.hpp"

#include "utilities/spch.hpp"
#include <glad/glad.h>


namespace siren::core
{
FrameBuffer::FrameBuffer(const Properties& properties) : m_properties(properties) { create(); }

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &m_handle.value);
}

const FrameBuffer::Properties& FrameBuffer::properties() const { return m_properties; }

void FrameBuffer::bind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_handle.value);
}

void FrameBuffer::unbind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

FrameBufferHandle FrameBuffer::handle() const { return m_handle; }

void FrameBuffer::set_viewport() const {
    glViewport(0, 0, m_properties.width, m_properties.height);
}

std::optional<ImageHandle> FrameBuffer::color_attachment_handle() const {
    if (!m_color) { return std::nullopt; }
    return m_color->image.handle();
}

std::optional<ImageHandle> FrameBuffer::depth_attachment_handle() const {
    if (!m_depth) { return std::nullopt; }
    return m_depth->image.handle();
}

std::optional<ImageHandle> FrameBuffer::stencil_attachment_handle() const {
    if (!m_stencil) { return std::nullopt; }
    return m_stencil->image.handle();
}

void FrameBuffer::resize(const u32 width, const u32 height) {
    if (m_handle != 0) {
        glDeleteFramebuffers(1, &m_handle);
        m_handle = 0;
    }

    // update properties
    m_properties.width  = width;
    m_properties.height = height;

    // invalidate old data
    if (m_color) m_color.reset();
    if (m_depth) m_depth.reset();
    if (m_stencil) m_stencil.reset();

    // regenerate
    create();
    Logger::renderer->debug("Framebuffer resized to: ({}, {})", m_properties.width, m_properties.height);
}

void FrameBuffer::create() {
    // need to have at least one attachment
    SIREN_ASSERT(
        m_properties.hasColorBuffer || m_properties.hasDepthBuffer ||
        m_properties.hasStencilBuffer,
        "FrameBuffer must be created with at least one buffer attachment"
    );

    glCreateFramebuffers(1, &m_handle.value);

    // create attachments

    if (m_properties.hasColorBuffer) {
        m_color = std::make_shared<Texture>(
            "Color Attachment",
            ImageFormat::LinearColor8,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0, m_color->GetID(), 0);
    }

    if (m_properties.hasDepthBuffer) {
        m_depth = create_own<Texture2D>(
            "Depth Attachment",
            ImageFormat::DepthStencil,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_handle, GL_DEPTH_ATTACHMENT, m_depth->GetID(), 0);
    }

    if (m_properties.hasStencilBuffer) {
        m_stencil = create_own<Texture2D>(
            "Stencil Attachment",
            ImageFormat::DepthStencil,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_handle,GL_STENCIL_ATTACHMENT, m_stencil->GetID(), 0);
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }
}
} // namespace siren::core
