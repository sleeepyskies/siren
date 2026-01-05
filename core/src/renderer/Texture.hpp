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
 * @brief The format of the image. Manages both the CPU layout and
 * how the texture is stored in VRAM.
 */
enum class ImageFormat
{
    Mask8,        ///< 1-Channel byte data.
    LinearColor8, ///< 4-Channel (RGBA) byte data. (linear encoding)
    Color8,       ///< 4-Channel (sRGBA) byte data. (sRGB encoding)
    Hdr16,        ///< 3-Channel (RGB) HDR float data.
    DepthStencil, ///< Depth Stencil Buffer data
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
    explicit Texture(const std::string& name, ImageFormat format);

    /**
     * @brief Sets this texture to the currently active texture, as well as binds the texture to
     * the slot passed in.
     * @param location The slot/unit to bind to, 0 by default
     */
    virtual void Attach(u32 location = 0) const = 0;
    virtual void Detach(u32 location) const = 0;

    u32 GetID() const { return m_id; }

protected:
    /// @brief OpenGL ID
    u32 m_id = 0;
    ImageFormat m_format;
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
        ImageFormat format,
        u32 width,
        u32 height
    );

    /// @brief Used to create an empty texture.
    Texture2D(
        const std::string& name,
        ImageFormat format,
        u32 width,
        u32 height
    );
    ~Texture2D() override;

    /// @brief Returns the width of the texture in pixels.
    u32 GetWidth() const { return m_width; }

    /// @brief Returns the height of the texture in pixels.
    u32 GetHeight() const { return m_height; }

    void Attach(u32 location) const override;
    void Detach(u32 location) const override;

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
        ImageFormat format,
        u32 size
    );

    /// @brief Returns the size of the texture in pixels.
    u32 GetSize() const { return m_size; }

    void Attach(u32 location) const override;
    void Detach(u32 location) const override;

private:
    /// @brief Height and width of each face in pixels
    u32 m_size = 0;
};
} // namespace siren::core
