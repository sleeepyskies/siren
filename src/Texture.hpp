#pragma once

#include <glad/glad.h>
#include <stb_image.hpp>

#include "slog.hpp"

// OpenGL images can be a little confusing. We have:
// - Texture, this is a block of GPU memory for the actual texture data
// - Texture unit, which is one of n slots that can hold a bound texture, meaning the GPU can use it
// when sampling
// - Uniform sampler, which is a uniform and how we access the texture from within a shader

/**
 * @brief Represents a block of GPU memory holding image data.
 */
class Texture2D
{
public:
    /**
     * @brief How the GPU should handle texels larger or smaller than a projected pixel.
     */
    enum Filtering {
        NEAREST = GL_NEAREST,
        // ...
    };

    /**
     * @brief How the GPU should handle out of bounds UV coordinates.
     */
    enum WrapMode {
        REPEAT = GL_REPEAT,
        // ...
    };

    enum ImageFormat {
        RED  = GL_RED,
        RG   = GL_RG,
        RGB  = GL_RGB,
        RGBA = GL_RGBA,
    };

    explicit Texture2D(const std::string& filePath);
    Texture2D(const std::vector<unsigned char>& data, int width, int height, int channels);
    ~Texture2D();

    GLuint id() const;

    /**
     * @brief Sets this texture to the currently active texture, as well as binds the texture to
     * the slot passed in.
     * @param slot The slot/unit to bind to, 0 by default
     */
    void bind(const uint8_t slot = 0) const;
    void unbind() const;

private:
    GLuint m_id    = 0;
    int m_width    = 0;
    int m_height   = 0;
    int m_channels = 0;
};
