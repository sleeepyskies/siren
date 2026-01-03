#include "Texture.hpp"


namespace siren::core
{
// ============================================================================
// == MARK: Texture2D
// ============================================================================

Texture::Texture(const std::string& name) : Asset(name) { }

Texture2D::Texture2D(
    const std::string& name,
    const Vector<u8>& data,
    const TextureSampler& sampler,
    const u32 width,
    const u32 height
)
    : Texture(name), m_width(width), m_height(height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minification));
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magnification));

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));

    // upload data
    DataFormat dataFormat;
    switch (data.size() / width / height) {
        case 1: dataFormat = DataFormat::Red;
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

    // image is w x h, and stores data as a 4 byte rgba, with 1 mip map level
    glTextureStorage2D(m_id, 1, static_cast<GLint>(internalFormat), width, height);
    // upload the data to the image
    glTextureSubImage2D(m_id, 0, 0, 0, width, height, static_cast<GLint>(dataFormat), GL_UNSIGNED_BYTE, data.data());

    trc("Created Texture2D");
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
    : Texture(name), m_width(width), m_height(height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    i32 dataType;

    switch (dataFormat) {
        case DataFormat::Depth: {
            dataType = GL_FLOAT;
            break;
        }
        case DataFormat::DepthStencil: {
            dataType = GL_UNSIGNED_INT_24_8;
            break;
        }
        default: {
            dataType = GL_UNSIGNED_BYTE;
            break;
        }
    }

    // image is w x h, and stores data as a 4 byte rgba, with 1 mip map level
    glTextureStorage2D(m_id, 1, static_cast<GLint>(internalFormat), width, height);

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

void Texture2D::detach(const u32 location) const
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
    const u32 size
) : Texture(name), m_size(size)
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);

    // texture sampling alg
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minification));
    glTextureParameteri(m_id,GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magnification));

    // texture out of bounds behaviour
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.sWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.tWrap));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, static_cast<GLint>(sampler.rWrap));

    // upload data
    DataFormat dataFormat;
    switch (data[0].size() / (m_size * m_size)) {
        case 1: dataFormat = DataFormat::Red;
            break;
        case 2: dataFormat = DataFormat::RG;
            break;
        case 3: dataFormat = DataFormat::RGB;
            break;
        case 4: dataFormat = DataFormat::RGBA;
            break;
        default: SirenAssert(false, "Invalid TextureCubeMap parameters.");
    }
    auto internalFormat = InternalFormat::RGBA8;

    glTextureStorage2D(m_id, 1, static_cast<GLuint>(internalFormat), size, size);
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
            static_cast<GLenum>(dataFormat),
            GL_UNSIGNED_BYTE,
            data[i].data()
        );
    }

    trc("Created TextureCubeMap");
}

void TextureCubeMap::Attach(const u32 location) const
{
    glBindTextureUnit(location, m_id);
}

void TextureCubeMap::detach(const u32 location) const
{
    glBindTextureUnit(location, 0);
}
} // namespace siren::core
