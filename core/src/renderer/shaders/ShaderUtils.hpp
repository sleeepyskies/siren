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
        case VertexAttribute::Color: return "a_color";
    }
    IllegalState;
}

/// @brief Maps a ShaderAttribute to its amount of components
constexpr u32 toComponentCount(const VertexAttribute attribute)
{
    switch (attribute) {
        case VertexAttribute::Position: return 3;
        case VertexAttribute::Normal: return 3;
        case VertexAttribute::Tangent: return 3;
        case VertexAttribute::Bitangent: return 3;
        case VertexAttribute::Texture: return 2;
        case VertexAttribute::Color: return 4;
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
        case VertexAttribute::Texture:
        case VertexAttribute::Color: return GL_FLOAT;
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

constexpr GLenum depthFunctionToEnum(const DepthFunction df)
{
    switch (df) {
        case DepthFunction::Always: return GL_ALWAYS;
        case DepthFunction::Never: return GL_NEVER;
        case DepthFunction::Less: return GL_LESS;
        case DepthFunction::Equal: return GL_EQUAL;
        case DepthFunction::LessEqual: return GL_LEQUAL;
        case DepthFunction::Greater: return GL_GREATER;
        case DepthFunction::NotEqual: return GL_NOTEQUAL;
        case DepthFunction::GreaterEqual: return GL_GEQUAL;
    }
    IllegalState;
}
} // namespace siren::core
