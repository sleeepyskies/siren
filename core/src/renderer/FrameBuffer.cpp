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

void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::prepare() const
{
    setViewport();
    clearBuffers();
}

void FrameBuffer::setViewport() const
{
    glViewport(0, 0, m_properties.width, m_properties.height);
}

void FrameBuffer::setClearColor(const glm::vec4 color)
{
    m_properties.clearColor = color;
}

void FrameBuffer::clearBuffers() const
{
    bind();
    const glm::vec4 color = m_properties.clearColor;
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FrameBuffer::resize(const u32 width, const u32 height)
{
    // update properties
    m_properties.width  = width;
    m_properties.height = height;

    // invalidate old data
    m_color   = nullptr;
    m_depth   = nullptr;
    m_stencil = nullptr;
    m_id      = 0;

    // regenerate
    create();
}

void FrameBuffer::create()
{
    // need to have at least one attachment
    SirenAssert(m_properties.hasColorBuffer || m_properties.hasDepthBuffer ||
                    m_properties.hasStencilBuffer,
                "FrameBuffer must be created with at least one buffer attachment");

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // create attachments

    if (m_properties.hasColorBuffer) {
        m_color = createOwn<Texture2D>(m_properties.width,
                                       m_properties.height,
                                       Texture2D::InternalFormat::RGBA8,
                                       Texture2D::DataFormat::RGBA);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color->id(), 0);
    }

    if (m_properties.hasDepthBuffer) {
        m_depth = createOwn<Texture2D>(m_properties.width,
                                       m_properties.height,
                                       Texture2D::InternalFormat::DEPTH24,
                                       Texture2D::DataFormat::DEPTH);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->id(), 0);
    }

    if (m_properties.hasStencilBuffer) {
        m_stencil = createOwn<Texture2D>(m_properties.width,
                                         m_properties.height,
                                         Texture2D::InternalFormat::STENCIL8,
                                         Texture2D::DataFormat::STENCIL);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_stencil->id(), 0);
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }

    unbind();
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

} // namespace siren::core