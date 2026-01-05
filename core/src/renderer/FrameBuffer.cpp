#include "FrameBuffer.hpp"

#include "utilities/spch.hpp"
#include <glad/glad.h>


namespace siren::core
{
FrameBuffer::FrameBuffer(const Properties& properties) : m_properties(properties)
{
    Create();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_id);
}

const FrameBuffer::Properties& FrameBuffer::getProperties() const
{
    return m_properties;
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

u32 FrameBuffer::GetID() const
{
    return m_id;
}

void FrameBuffer::SetViewport() const
{
    glViewport(0, 0, m_properties.width, m_properties.height);
}

Maybe<u32> FrameBuffer::GetColorAttachmentID() const
{
    if (!m_color) { return Nothing; }
    return m_color->GetID();
}

Maybe<u32> FrameBuffer::GetDepthAttachmentID() const
{
    if (!m_depth) { return Nothing; }
    return m_depth->GetID();
}

Maybe<u32> FrameBuffer::GetStencilAttachmentID() const
{
    if (!m_stencil) { return Nothing; }
    return m_stencil->GetID();
}

void FrameBuffer::Resize(const u32 width, const u32 height)
{
    if (m_id != 0) {
        glDeleteFramebuffers(1, &m_id);
        m_id = 0;
    }

    // update properties
    m_properties.width  = width;
    m_properties.height = height;

    // invalidate old data
    if (m_color) m_color.reset();
    if (m_depth) m_depth.reset();
    if (m_stencil) m_stencil.reset();

    // regenerate
    Create();
    dbg("Framebuffer resized to: ({}, {})", m_properties.width, m_properties.height);
}

void FrameBuffer::Create()
{
    // need to have at least one attachment
    SirenAssert(
        m_properties.hasColorBuffer || m_properties.hasDepthBuffer ||
        m_properties.hasStencilBuffer,
        "FrameBuffer must be created with at least one buffer attachment"
    );

    glCreateFramebuffers(1, &m_id);

    // create attachments

    if (m_properties.hasColorBuffer) {
        m_color = CreateOwn<Texture2D>(
            "Color Attachment",
            ImageFormat::LinearColor8,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0, m_color->GetID(), 0);
    }

    if (m_properties.hasDepthBuffer) {
        m_depth = CreateOwn<Texture2D>(
            "Depth Attachment",
            ImageFormat::DepthStencil,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depth->GetID(), 0);
    }

    if (m_properties.hasStencilBuffer) {
        m_stencil = CreateOwn<Texture2D>(
            "Stencil Attachment",
            ImageFormat::DepthStencil,
            m_properties.width,
            m_properties.height
        );
        glNamedFramebufferTexture(m_id,GL_STENCIL_ATTACHMENT, m_stencil->GetID(), 0);
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }
}
} // namespace siren::core
