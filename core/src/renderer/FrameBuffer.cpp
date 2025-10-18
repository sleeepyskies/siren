#include "FrameBuffer.hpp"

#include <glad/glad.h>

namespace siren::renderer
{

FrameBuffer::FrameBuffer(const Properties& properties) : m_properties(properties)
{
    // need to have at least one attachment
    SirenAssert(properties.hasColorBuffer || properties.hasDepthBuffer ||
                    properties.hasStencilBuffer,
                "FrameBuffer must be created with at least one color attachment");

    glGenFramebuffers(1, &m_id);

    // create color attachments
    if (properties.hasColorBuffer) {
        // color
        m_color = makeUref<Texture2D>(properties.width,
                                      properties.height,
                                      Texture2D::InternalFormat::RGBA8,
                                      Texture2D::DataFormat::RGBA);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color->id(), 0);
    }

    if (properties.hasDepthBuffer) {
        // depth
        m_depth = makeUref<Texture2D>(properties.width,
                                      properties.height,
                                      Texture2D::InternalFormat::DEPTH24,
                                      Texture2D::DataFormat::DEPTH);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->id(), 0);
    }

    if (properties.hasStencilBuffer) {
        if (properties.hasStencilBuffer) {
            // stencil
            m_stencil = makeUref<Texture2D>(properties.width,
                                            properties.height,
                                            Texture2D::InternalFormat::STENCIL8,
                                            Texture2D::DataFormat::STENCIL);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_stencil->id(), 0);
        }
    }

    // check everything worked
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SirenAssert(false, "FrameBuffer could not be created");
    }

    unbind();
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

} // namespace siren::renderer