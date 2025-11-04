#include "FrameBuffer.hpp"

#include "utilities/spch.hpp"
#include <glad/glad.h>


namespace siren::core
{

FrameBuffer::FrameBuffer(const Properties& properties) : m_properties(properties)
{
    create();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_id);
}

const FrameBuffer::Properties& FrameBuffer::getProperties() const
{
    return m_properties;
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

u32 FrameBuffer::getId() const
{
    return m_id;
}

void FrameBuffer::setViewport() const
{
    glViewport(0, 0, m_properties.width, m_properties.height);
}

Maybe<u32> FrameBuffer::getColorAttachmentId() const
{
    if (!m_color) {
        return Nothing;
    }
    return m_color->id();
}

Maybe<u32> FrameBuffer::getDepthAttachmentId() const
{
    if (!m_depth) {
        return Nothing;
    }
    return m_depth->id();
}

Maybe<u32> FrameBuffer::getStencilAttachmentId() const
{
    if (!m_stencil) {
        return Nothing;
    }
    return m_stencil->id();
}

void FrameBuffer::resize(const u32 width, const u32 height)
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
    create();
    dbg("Framebuffer resized to: ({}, {})", m_properties.width, m_properties.height);
}

void FrameBuffer::create()
{
    // need to have at least one attachment
    SirenAssert(
        m_properties.hasColorBuffer || m_properties.hasDepthBuffer ||
        m_properties.hasStencilBuffer,
        "FrameBuffer must be created with at least one buffer attachment"
    );

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // create attachments

    if (m_properties.hasColorBuffer) {
        m_color = createOwn<Texture2D>(
            "Color Attachment",
            m_properties.width,
            m_properties.height,
            InternalFormat::RGBA8,
            DataFormat::RGBA
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_color->id(),
            0
        );
    }

    if (m_properties.hasDepthBuffer) {
        m_depth = createOwn<Texture2D>(
            "Depth Attachment",
            m_properties.width,
            m_properties.height,
            InternalFormat::DEPTH24,
            DataFormat::DEPTH
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            m_depth->id(),
            0
        );
    }

    if (m_properties.hasStencilBuffer) {
        m_stencil = createOwn<Texture2D>(
            "Stencil Attachment",
            m_properties.width,
            m_properties.height,
            InternalFormat::STENCIL8,
            DataFormat::STENCIL
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            m_stencil->id(),
            0
        );
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }

    unbind();
}

} // namespace siren::core
