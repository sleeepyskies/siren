#include "Texture.hpp"

namespace core::renderer
{

Texture2D::Texture2D(const std::vector<Byte>& data, const std::string& name,
                     const Image2DSampler sampler, const int w, const int h)
    : Asset(name), m_width(w), m_height(h)
{
    // we infer the channels
    const int c = data.size() / w / h;

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
    InternalFormat internalFormat;
    switch (c) {
    case 1:
        internalFormat = InternalFormat::R8;
        dataFormat     = ImageFormat::RED;
        break;
    case 2:
        internalFormat = InternalFormat::RG8;
        dataFormat     = ImageFormat::RG;
        break;
    case 3:
        internalFormat = InternalFormat::RGB8;
        dataFormat     = ImageFormat::RGB;
        break;
    default:
        internalFormat = InternalFormat::RGBA8;
        dataFormat     = ImageFormat::RGBA;
        break;
    }

    // internal format := how it should be stored
    // format          := how it is stored now
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

    trc("Created Texture2D Object {} with {} channels.", name, c);
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

} // namespace core::renderer