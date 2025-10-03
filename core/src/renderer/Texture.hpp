#pragma once

#include "core/GL.hpp"
#include "utilities/spch.hpp"

#include <assets/Asset.hpp>

namespace core::renderer
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
    enum class ImageFormat {
        RED  = GL_RED,
        RG   = GL_RG,
        RGB  = GL_RGB,
        RGBA = GL_RGBA,
    };

    enum class InternalFormat {
        R8    = GL_R8, //
        RG8   = GL_RG8,
        RGB8  = GL_RGB8,
        RGBA8 = GL_RGBA8
    };

    Texture2D(const std::vector<Byte>& data, Image2DSampler sampler, int w, int h);
    ~Texture2D();

    /**
     * @brief Sets this texture to the currently active texture, as well as binds the texture to
     * the slot passed in.
     * @param slot The slot/unit to bind to, 0 by default
     */
    void bind(uint8_t slot = 0) const;
    void unbind() const;

private:
    /// @brief OpenGL ID
    GLuint m_id    = 0;
    /// @brief Width in pixel
    int m_width    = 0;
    /// @brief Height in pixel
    int m_height   = 0;
    /// @brief Number of color channels (r/g/b/a)
    int m_channels = 0;
};

} // namespace core::renderer
