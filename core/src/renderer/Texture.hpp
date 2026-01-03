#pragma once

#include "assets/Asset.hpp"
#include "platform/GL.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
struct TextureSampler
{
    enum class Filtering
    {
        Nearest = GL_NEAREST,
        Linear  = GL_LINEAR,
    };

    enum class WrapMode
    {
        Repeat = GL_REPEAT,
        Clamp  = GL_CLAMP_TO_EDGE,
        Mirror = GL_MIRRORED_REPEAT,
    };

    Filtering minification  = Filtering::Nearest;
    Filtering magnification = Filtering::Nearest;
    WrapMode sWrap          = WrapMode::Repeat;
    WrapMode tWrap          = WrapMode::Repeat;
    WrapMode rWrap          = WrapMode::Repeat;
};

/**
 * @brief Specifies the format of how the image data should be stored GPU side.
 */
enum class DataFormat
{
    Red          = GL_RED,
    RG           = GL_RG,
    RGB          = GL_RGB,
    RGBA         = GL_RGBA,
    Depth        = GL_DEPTH_COMPONENT,
    Stencil      = GL_STENCIL_INDEX,
    DepthStencil = GL_DEPTH_STENCIL,
};

/**
 * @brief Specifies the format of the source image data CPU side.
 * @todo Add HDR Support
 */
enum class InternalFormat
{
    R8       = GL_R8,
    RG8      = GL_RG8,
    RGB8     = GL_RGB8,
    RGBA8    = GL_RGBA8,
    Depth24  = GL_DEPTH_COMPONENT24,
    Stencil8 = GL_STENCIL_INDEX8,
};

enum class CubeMapFace
{
    PositiveX,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ
};

// OpenGL images can be a little confusing. We have:
// - Texture, this is a block of GPU memory for the actual texture data
// - Texture unit, which is one of n slots that can hold a bound texture, meaning the GPU can use it
// when sampling
// - Uniform sampler, which is a uniform and how we access the texture from within a shader

class Texture : public Asset
{
public:
    explicit Texture(const std::string& name);

    /**
     * @brief Sets this texture to the currently active texture, as well as binds the texture to
     * the slot passed in.
     * @param location The slot/unit to bind to, 0 by default
     */
    virtual void Attach(u32 location = 0) const = 0;
    virtual void detach(u32 location) const = 0;

    u32 id() const { return m_id; }

protected:
    /// @brief OpenGL ID
    u32 m_id = 0;
};

/**
 * @brief Represents a block of GPU memory holding image data.
 */
class Texture2D final : public Texture
{
public:
    ASSET_TYPE(AssetType::Texture2D);

    /// @brief Used to create a texture with some texture data.
    Texture2D(
        const std::string& name,
        const Vector<u8>& data,
        const TextureSampler& sampler,
        u32 width,
        u32 height
    );

    /// @brief Used to create an empty texture.
    Texture2D(
        const std::string& name,
        u32 width,
        u32 height,
        InternalFormat internalFormat,
        DataFormat dataFormat
    );
    ~Texture2D() override;

    /// @brief Returns the width of the texture in pixels.
    u32 getWidth() const { return m_width; }

    /// @brief Returns the height of the texture in pixels.
    u32 getHeight() const { return m_height; }

    void Attach(u32 location) const override;
    void detach(u32 location) const override;

private:
    /// @brief Width in pixels.
    u32 m_width = 0;
    /// @brief Height in pixels.
    u32 m_height = 0;
};

class TextureCubeMap final : public Texture
{
public:
    ASSET_TYPE(AssetType::TextureCubeMap);

    TextureCubeMap(
        const std::string& name,
        const Array<Vector<u8>, 6>& data,
        const TextureSampler& sampler,
        u32 size
    );

    /// @brief Returns the size of the texture in pixels.
    u32 getSize() const { return m_size; }

    void Attach(u32 location) const override;
    void detach(u32 location) const override;

private:
    /// @brief Height and width of each face in pixels
    u32 m_size = 0;
};
} // namespace siren::core
