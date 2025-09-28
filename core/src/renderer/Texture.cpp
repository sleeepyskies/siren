#include "Texture.hpp"

#include <stb_image.h>

namespace core
{

Texture2D::Texture2D(const std::string& filePath)
{
    stbi_set_flip_vertically_on_load(true); // stb loads images upside down for opengl
    stbi_uc* data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channels, STBI_rgb_alpha);
    if (!data) err("Could not load texture from {}", filePath);

    glGenTextures(1, &m_id);
    bind(0);

    // texture sampling alg
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture out of bounds behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // upload data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    // create mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();

    trc("Created Texture2D Object.");
}

Texture2D::Texture2D(const std::string& name, const std::vector<unsigned char>& data,
                     const int width, const int height, const int channels)
    : m_name(name), m_width(width), m_height(height), m_channels(channels)
{

    glGenTextures(1, &m_id);
    bind(0);

    // texture sampling alg
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture out of bounds behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // upload data
    ImageFormat dataFormat;
    InternalFormat internalFormat;
    switch (channels) {
    case 1:
        internalFormat = R8;
        dataFormat     = RED;
        break;
    case 2:
        internalFormat = RG8;
        dataFormat     = RG;
        break;
    case 3:
        internalFormat = RGB8;
        dataFormat     = RGB;
        break;
    default:
        internalFormat = RGBA8;
        dataFormat     = RGBA;
        break;
    }

    // internal format := how it should be stored
    // format          := how it is stored now
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 width,
                 height,
                 0,
                 dataFormat,
                 GL_UNSIGNED_BYTE,
                 data.data());
    // create mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();

    trc("Created Texture2D Object {} with {} channels.", name, channels);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

GLuint Texture2D::id() const
{
    return m_id;
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

} // namespace core