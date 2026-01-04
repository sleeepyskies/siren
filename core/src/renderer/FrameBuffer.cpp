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
    return m_color->id();
}

Maybe<u32> FrameBuffer::GetDepthAttachmentID() const
{
    if (!m_depth) { return Nothing; }
    return m_depth->id();
}

Maybe<u32> FrameBuffer::GetStencilAttachmentID() const
{
    if (!m_stencil) { return Nothing; }
    return m_stencil->id();
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
            m_properties.width,
            m_properties.height,
            InternalFormat::RGBA8,
            DataFormat::RGBA
        );
        glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0, m_color->id(), 0);
    }

    if (m_properties.hasDepthBuffer) {
        m_depth = CreateOwn<Texture2D>(
            "Depth Attachment",
            m_properties.width,
            m_properties.height,
            InternalFormat::Depth24,
            DataFormat::Depth
        );
        glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depth->id(), 0);
    }

    if (m_properties.hasStencilBuffer) {
        m_stencil = CreateOwn<Texture2D>(
            "Stencil Attachment",
            m_properties.width,
            m_properties.height,
            InternalFormat::Stencil8,
            DataFormat::Stencil
        );
        glNamedFramebufferTexture(m_id,GL_STENCIL_ATTACHMENT, m_stencil->id(), 0);
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }
}
} // namespace siren::core
