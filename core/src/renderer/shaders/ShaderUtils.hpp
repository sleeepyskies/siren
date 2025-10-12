#pragma once

#include "renderer/buffer/VertexBufferLayout.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

/// @brief Maps a ShaderAttribute to a string
constexpr const char* toString(const ShaderAttribute attribute)
{
    switch (attribute) {
        case ShaderAttribute::POSITION : return "aPosition";
        case ShaderAttribute::NORMAL   : return "aNormal";
        case ShaderAttribute::TANGENT  : return "aTangent";
        case ShaderAttribute::BITANGENT: return "aBitangent";
        case ShaderAttribute::TEXTUREUV: return "aTextureUV";
        case ShaderAttribute::COLOR    : return "aColor";
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

/// @brief Maps a ShaderAttribute to its amount of components
constexpr int32_t toComponentCount(const ShaderAttribute attribute)
{
    switch (attribute) {
        case ShaderAttribute::POSITION : return 3;
        case ShaderAttribute::NORMAL   : return 3;
        case ShaderAttribute::TANGENT  : return 3;
        case ShaderAttribute::BITANGENT: return 3;
        case ShaderAttribute::TEXTUREUV: return 2;
        case ShaderAttribute::COLOR    : return 4;
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

/// @brief Maps a ShaderAttribute to its data type
constexpr GLenum toGLType(const ShaderAttribute attribute)
{
    switch (attribute) {
        case ShaderAttribute::POSITION:
        case ShaderAttribute::NORMAL:
        case ShaderAttribute::TANGENT:
        case ShaderAttribute::BITANGENT:
        case ShaderAttribute::TEXTUREUV:
        case ShaderAttribute::COLOR    : return GL_FLOAT;
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

} // namespace siren::renderer
