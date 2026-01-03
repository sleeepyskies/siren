#pragma once

#include "renderer/GraphicsPipeline.hpp"
#include "renderer/buffer/VertexLayout.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
/// @brief Maps a ShaderAttribute to a string
constexpr const char* toString(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::Position: return "a_position";
        case VertexAttribute::Normal: return "a_normal";
        case VertexAttribute::Tangent: return "a_tangent";
        case VertexAttribute::Bitangent: return "a_bitangent";
        case VertexAttribute::Texture: return "a_textureuv";
    }
    IllegalState;
}

/// @brief Maps a ShaderAttribute to its amount of components
constexpr int32_t toComponentCount(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::Position: return 3;
        case VertexAttribute::Normal: return 3;
        case VertexAttribute::Tangent: return 3;
        case VertexAttribute::Bitangent: return 3;
        case VertexAttribute::Texture: return 2;
    }
    IllegalState;
}

/// @brief Maps a ShaderAttribute to its data type
constexpr GLenum toGLType(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::Position:
        case VertexAttribute::Normal:
        case VertexAttribute::Tangent:
        case VertexAttribute::Bitangent:
        case VertexAttribute::Texture: return GL_FLOAT;
    }
    IllegalState;
}

constexpr GLenum topologyToGlEnum(const PrimitiveTopology top)
{
    switch (top) {
        case PrimitiveTopology::Points: return GL_POINTS;
        case PrimitiveTopology::Lines: return GL_LINES;
        case PrimitiveTopology::Triangles: return GL_TRIANGLES;
        case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
        case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan: return GL_TRIANGLE_FAN;
    }
    IllegalState;
}
} // namespace siren::core
