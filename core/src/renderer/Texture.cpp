#include "Texture.hpp"

namespace siren::renderer
{

Texture2D::Texture2D(const std::vector<Byte>& data, const Image2DSampler sampler, const int w,
                     const int h)
    : m_width(w), m_height(h), m_channels(data.size() / w / h)
{
    glGenTextures(1, &m_id);
    bind(0);

    // texture sampling alg
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minification));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magnification));

    // texture out of bounds behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));

    // upload data
    ImageFormat dataFormat;
    switch (m_channels) {
        case 1 : dataFormat = ImageFormat::RED; break;
        case 2 : dataFormat = ImageFormat::RG; break;
        case 3 : dataFormat = ImageFormat::RGB; break;
        default: dataFormat = ImageFormat::RGBA; break;
    }
    auto internalFormat = InternalFormat::RGBA8;

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 static_cast<GLint>(internalFormat),
                 w,
                 h,
                 0,
                 static_cast<GLenum>(dataFormat),
                 GL_UNSIGNED_BYTE,
                 data.data());
    // create mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();

    trc("Created Texture2D Object");
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

void Texture2D::bind(const uint8_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace siren::renderer