#pragma once

#include "../platform/GL.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

struct Image2DSampler {
    enum class Filtering {
        NEAREST = GL_NEAREST,
        LINEAR  = GL_LINEAR,
    };

    enum class WrapMode {
        REPEAT = GL_REPEAT,
        CLAMP  = GL_CLAMP_TO_EDGE,
        MIRROR = GL_MIRRORED_REPEAT,
    };

    Filtering minification  = Filtering::NEAREST;
    Filtering magnification = Filtering::NEAREST;
    WrapMode sWrap          = WrapMode::REPEAT;
    WrapMode tWrap          = WrapMode::REPEAT;
};

// OpenGL images can be a little confusing. We have:
// - Texture, this is a block of GPU memory for the actual texture data
// - Texture unit, which is one of n slots that can hold a bound texture, meaning the GPU can use it
// when sampling
// - Uniform sampler, which is a uniform and how we access the texture from within a shader

/**
 * @brief Represents a block of GPU memory holding image data.
 */
class Texture2D final
{
public:
    /**
     * @brief Specifies the format of how the image data should be stored GPU side.
     */
    enum class DataFormat {
        RED           = GL_RED,
        RG            = GL_RG,
        RGB           = GL_RGB,
        RGBA          = GL_RGBA,
        DEPTH         = GL_DEPTH_COMPONENT,
        STENCIL       = GL_STENCIL_INDEX,
        DEPTH_STENCIL = GL_DEPTH_STENCIL,
    };

    /**
     * @brief Specifies the format of the source image data CPU side.
     * @todo Add HDR Support
     */
    enum class InternalFormat {
        R8       = GL_R8, //
        RG8      = GL_RG8,
        RGB8     = GL_RGB8,
        RGBA8    = GL_RGBA8,
        DEPTH24  = GL_DEPTH_COMPONENT24,
        STENCIL8 = GL_STENCIL_INDEX8,
    };

    /// @brief Used to create a texture with some texture data
    Texture2D(const std::vector<Byte>& data, Image2DSampler sampler, uint32_t width,
              uint32_t height);
    /// @brief Used to an empty texture
    Texture2D(uint32_t width, uint32_t height, InternalFormat internalFormat,
              DataFormat dataFormat);
    ~Texture2D();

    /**
     * @brief Sets this texture to the currently active texture, as well as binds the texture to
     * the slot passed in.
     * @param location The slot/unit to bind to, 0 by default
     */
    void attach(uint8_t location = 0) const;
    void unbind() const;

    uint32_t id() const;

private:
    /// @brief OpenGL ID
    uint32_t m_id     = 0;
    /// @brief Width in pixels
    uint32_t m_width  = 0;
    /// @brief Height in pixels
    uint32_t m_height = 0;
};

} // namespace siren::renderer
