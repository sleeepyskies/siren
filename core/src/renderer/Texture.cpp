#include "Texture.hpp"

namespace siren::core
{

Texture2D::Texture2D(
    const std::string& name,
    const std::vector<u8>& data,
    const Texture2DSampler sampler,
    const u32 width,
    const u32 height)
    : Asset(name), m_width(width), m_height(height)
{
    glGenTextures(1, &m_id);
    attach(0);

    // texture sampling alg
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minification));
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        static_cast<GLint>(sampler.magnification)
        );

    // texture out of bounds behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));

    // upload data
    DataFormat dataFormat;
    switch (data.size() / width / height) {
        case 1: dataFormat = DataFormat::RED;
            break;
        case 2: dataFormat = DataFormat::RG;
            break;
        case 3: dataFormat = DataFormat::RGB;
            break;
        case 4: dataFormat = DataFormat::RGBA;
            break;
        default: SirenAssert(false, "Invalid Texture2D parameters.");
    }
    auto internalFormat = InternalFormat::RGBA8;

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 static_cast<GLint>(internalFormat),
                 width,
                 height,
                 0,
                 static_cast<GLenum>(dataFormat),
                 GL_UNSIGNED_BYTE,
                 data.data());
    // create mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();

    trc("Created Texture2D Object");
}

// TODO:
//  - maybe we can create a storage object instead of texture? glTextureStorage()?
//  - this means we dont need to sample in shaders and may be more efficient
Texture2D::Texture2D(
    const std::string& name,
    const u32 width,
    const u32 height,
    const InternalFormat internalFormat,
    const DataFormat dataFormat
    )
    : Asset(name), m_width(width), m_height(height)
{
    glGenTextures(1, &m_id);
    attach(0);

    // texture sampling alg
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture out of bounds behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (dataFormat == DataFormat::DEPTH) {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLint>(internalFormat),
            width,
            height,
            0,
            static_cast<GLenum>(dataFormat),
            GL_FLOAT,
            nullptr
            );
    } else if (dataFormat == DataFormat::DEPTH_STENCIL) {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLint>(internalFormat),
            width,
            height,
            0,
            static_cast<GLenum>(dataFormat),
            GL_UNSIGNED_INT_24_8,
            nullptr
            );
    } else {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLint>(internalFormat),
            width,
            height,
            0,
            static_cast<GLenum>(dataFormat),
            GL_UNSIGNED_BYTE,
            nullptr
            );
    }

    if (dataFormat != DataFormat::DEPTH && dataFormat != DataFormat::STENCIL &&
        dataFormat != DataFormat::DEPTH_STENCIL) {
        // dont generate mip map levels for depth/stencil buffers
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    unbind();

    trc("Created empty Texture2D Object");
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

void Texture2D::attach(const u8 location) const
{
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

u32 Texture2D::id() const
{
    return m_id;
}

} // namespace siren::core