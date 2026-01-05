#include "Texture.hpp"

#include "shaders/ShaderUtils.hpp"


namespace siren::core
{
// ============================================================================
// == MARK: Texture2D
// ============================================================================

Texture::Texture(const std::string& name, const ImageFormat format) : Asset(name), m_format(format) { }

Texture2D::Texture2D(
    const std::string& name,
    const Vector<u8>& data,
    const TextureSampler& sampler,
    const ImageFormat format,
    const u32 width,
    const u32 height
)
    : Texture(name, format), m_width(width), m_height(height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minification));
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magnification));

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));

    // image is w x h, and stores data as a 4 byte rgba, with 1 mip map level
    glTextureStorage2D(m_id, 1, imageFormatToInternalFormat(format), width, height);
    // upload the data to the image
    glTextureSubImage2D(
        m_id,
        0,
        0,
        0,
        width,
        height,
        imageFormatToDataFormat(format),
        imageFormatToDataType(format),
        data.data()
    );

    trc("Created Texture2D");
}

// TODO:
//  - maybe we can create a storage object instead of texture? glTextureStorage()?
//  - this means we dont need to sample in shaders and may be more efficient
Texture2D::Texture2D(
    const std::string& name,
    const ImageFormat format,
    const u32 width,
    const u32 height
)
    : Texture(name, format), m_width(width), m_height(height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // image is w x h, and stores data as a 4 byte rgba, with 1 mip map level
    glTextureStorage2D(m_id, 1, imageFormatToInternalFormat(format), width, height);

    trc("Created empty Texture2D Object");
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

void Texture2D::Attach(const u32 location) const
{
    glBindTextureUnit(location, m_id);
}

void Texture2D::Detach(const u32 location) const
{
    glBindTextureUnit(location, 0);
}

// ============================================================================
// == MARK: TextureCubeMap
// ============================================================================

TextureCubeMap::TextureCubeMap(
    const std::string& name,
    const Array<Vector<u8>, 6>& data,
    const TextureSampler& sampler,
    const ImageFormat format,
    const u32 size
) : Texture(name, format), m_size(size)
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_id,GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magnification));

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, static_cast<GLint>(sampler.rWrap));

    const u32 lodLevels = floor(log2(size)) + 1;

    glTextureStorage2D(m_id, lodLevels, imageFormatToInternalFormat(format), size, size);
    for (i32 i = 0; i < data.size(); i++) {
        glTextureSubImage3D(
            m_id,
            0,
            0,
            0,
            i,
            size,
            size,
            1,
            imageFormatToDataFormat(format),
            imageFormatToDataType(format),
            data[i].data()
        );
    }

    glGenerateMipmap(m_id);

    trc("Created TextureCubeMap");
}

void TextureCubeMap::Attach(const u32 location) const
{
    glBindTextureUnit(location, m_id);
}

void TextureCubeMap::Detach(const u32 location) const
{
    glBindTextureUnit(location, 0);
}
} // namespace siren::core
