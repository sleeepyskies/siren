#pragma once

#include "renderer/buffer/VertexBufferLayout.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

/// @brief Maps a ShaderAttribute to a string
constexpr const char* toString(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::POSITION : return "aPosition";
        case VertexAttribute::NORMAL   : return "aNormal";
        case VertexAttribute::TANGENT  : return "aTangent";
        case VertexAttribute::BITANGENT: return "aBitangent";
        case VertexAttribute::TEXTUREUV: return "aTextureUV";
        case VertexAttribute::COLOR    : return "aColor";
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

/// @brief Maps a ShaderAttribute to its amount of components
constexpr int32_t toComponentCount(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::POSITION : return 3;
        case VertexAttribute::NORMAL   : return 3;
        case VertexAttribute::TANGENT  : return 3;
        case VertexAttribute::BITANGENT: return 3;
        case VertexAttribute::TEXTUREUV: return 2;
        case VertexAttribute::COLOR    : return 4;
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

/// @brief Maps a ShaderAttribute to its data type
constexpr GLenum toGLType(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::POSITION:
        case VertexAttribute::NORMAL:
        case VertexAttribute::TANGENT:
        case VertexAttribute::BITANGENT:
        case VertexAttribute::TEXTUREUV:
        case VertexAttribute::COLOR    : return GL_FLOAT;
    }
    SirenAssert(false, "Invalid ShaderAttribute found");
}

} // namespace siren::core
