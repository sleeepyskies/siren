/// @file mappings.hpp
/// @brief Contains all Siren->OpenGL and OpenGL->Siren mappings.
#pragma once

#include "gl.hpp"
#include "renderer/resources/image.hpp"

/// @todo docs

namespace siren::platform::gl
{
constexpr auto img_filter_to_gl(const core::ImageFilterMode mode) -> GLenum {
    switch (mode) {
        case core::ImageFilterMode::Nearest: return GL_NEAREST;
        case core::ImageFilterMode::Linear: return GL_LINEAR;
        default: UNREACHABLE;
    }
}

// OpenGL is weird and chose to combine lod filtering and min filtering.

constexpr auto min_img_filter_to_gl(const core::ImageFilterMode min, const core::ImageFilterMode lod) -> GLenum {
    if (min == core::ImageFilterMode::Linear) {
        return lod == core::ImageFilterMode::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
    }
    return lod == core::ImageFilterMode::Linear ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
}

constexpr auto img_filter_to_siren(const GLenum mode) -> core::ImageFilterMode {
    switch (mode) {
        case GL_NEAREST: return core::ImageFilterMode::Nearest;
        case GL_LINEAR: return core::ImageFilterMode::Linear;
        default: UNREACHABLE;
    }
}

constexpr auto img_wrap_to_gl(const core::ImageWrapMode mode) -> GLenum {
    switch (mode) {
        case core::ImageWrapMode::Repeat: return GL_REPEAT;
        case core::ImageWrapMode::Mirror: return GL_MIRRORED_REPEAT;
        case core::ImageWrapMode::ClampEdge: return GL_CLAMP_TO_EDGE;
        case core::ImageWrapMode::ClampBorder: return GL_CLAMP_TO_BORDER;
        default: UNREACHABLE;
    }
}
constexpr auto img_wrap_to_siren(const GLenum mode) -> core::ImageWrapMode {
    switch (mode) {
        case GL_REPEAT: return core::ImageWrapMode::Repeat;
        case GL_MIRRORED_REPEAT: return core::ImageWrapMode::Mirror;
        case GL_CLAMP_TO_EDGE: return core::ImageWrapMode::ClampEdge;
        case GL_CLAMP_TO_BORDER: return core::ImageWrapMode::ClampBorder;
        default: UNREACHABLE;
    }
}

constexpr auto img_dim_to_gl(const core::ImageDimension dim) -> GLenum {
    switch (dim) {
        case core::ImageDimension::D1: return GL_TEXTURE_1D;
        case core::ImageDimension::D2: return GL_TEXTURE_2D;
        case core::ImageDimension::D3: return GL_TEXTURE_3D;
        default: UNREACHABLE;
    }
}

constexpr auto img_dim_to_siren(const GLenum dim) -> core::ImageDimension {
    switch (dim) {
        case GL_TEXTURE_1D: return core::ImageDimension::D1;
        case GL_TEXTURE_2D: return core::ImageDimension::D2;
        case GL_TEXTURE_3D: return core::ImageDimension::D3;
        default: UNREACHABLE;
    }
}

// This describes the gpu vram layout

constexpr auto img_format_to_gl_internal(const core::ImageFormat format) -> GLenum {
    switch (format) {
        case core::ImageFormat::Mask8: return GL_R8;
        case core::ImageFormat::LinearColor8: return GL_RGBA8;
        case core::ImageFormat::Color8: return GL_SRGB8_ALPHA8;
        case core::ImageFormat::Hdr16: return GL_RGB16F;
        case core::ImageFormat::DepthStencil: return GL_DEPTH24_STENCIL8;
        default: UNREACHABLE;
    }
}

// This describes the cpu side layout

constexpr auto img_format_to_gl_layout(const core::ImageFormat format) -> GLenum {
    switch (format) {
        case core::ImageFormat::Mask8: return GL_RED;
        case core::ImageFormat::LinearColor8:
        case core::ImageFormat::Color8: return GL_RGBA;
        case core::ImageFormat::Hdr16: return GL_RGB;
        case core::ImageFormat::DepthStencil: return GL_DEPTH_STENCIL;
        default: UNREACHABLE;
    }
}

constexpr auto img_format_from_gl_internal(const GLenum internal_format) -> core::ImageFormat {
    switch (internal_format) {
        case GL_R8: return core::ImageFormat::Mask8;
        case GL_RGBA8: return core::ImageFormat::LinearColor8;
        case GL_SRGB8_ALPHA8: return core::ImageFormat::Color8;
        case GL_RGB16F: return core::ImageFormat::Hdr16;
        case GL_DEPTH24_STENCIL8: return core::ImageFormat::DepthStencil;
        default: return core::ImageFormat::Unknown;
    }
}

/**
 * @brief Converts a siren @ref ImageCompareMode to its native GLint version.
 * @param mode The @ref ImageCompareMode to convert.
 * @return A converted GLint.
 */
constexpr auto img_compare_mode_to_gl(const core::ImageCompareMode mode) -> GLint {
    switch (mode) {
        case core::ImageCompareMode::None: return GL_NONE;
        case core::ImageCompareMode::Compare: return GL_COMPARE_REF_TO_TEXTURE;
        default: UNREACHABLE;
    }
}

/**
 * @brief Converts an OpenGL GLint to a siren ImageCompareMode.
 * @param mode The GLint to convert.
 * @return A converted siren @ref ImageCompareMode.
 */
constexpr auto img_compare_mode_to_siren(const GLint mode) -> core::ImageCompareMode {
    switch (mode) {
        case GL_NONE: return core::ImageCompareMode::None;
        case GL_COMPARE_REF_TO_TEXTURE: return core::ImageCompareMode::Compare;
        default: UNREACHABLE;
    }
}

/**
 * @brief Converts a siren @ref ImageCompareFn to its native GLenum version.
 * @param func The @ref ImageCompareFn to convert.
 * @return A converted GLenum.
 */
constexpr auto img_compare_fn_to_gl(const core::ImageCompareFn func) -> GLenum {
    switch (func) {
        case core::ImageCompareFn::Always: return GL_ALWAYS;
        case core::ImageCompareFn::Never: return GL_NEVER;
        case core::ImageCompareFn::Less: return GL_LESS;
        case core::ImageCompareFn::Equal: return GL_EQUAL;
        case core::ImageCompareFn::LessEqual: return GL_LEQUAL;
        case core::ImageCompareFn::Greater: return GL_GREATER;
        case core::ImageCompareFn::NotEqual: return GL_NOTEQUAL;
        case core::ImageCompareFn::GreaterEqual: return GL_GEQUAL;
        default: UNREACHABLE;
    }
}

/**
 * @brief Converts an OpenGL GLenum to a siren ImageCompareFn.
 * @param func The GLenum to convert.
 * @return A converted siren @ref ImageCompareFn.
 */
constexpr auto img_compare_fn_to_siren(const GLenum func) -> core::ImageCompareFn {
    switch (func) {
        case GL_ALWAYS: return core::ImageCompareFn::Always;
        case GL_NEVER: return core::ImageCompareFn::Never;
        case GL_LESS: return core::ImageCompareFn::Less;
        case GL_EQUAL: return core::ImageCompareFn::Equal;
        case GL_LEQUAL: return core::ImageCompareFn::LessEqual;
        case GL_GREATER: return core::ImageCompareFn::Greater;
        case GL_NOTEQUAL: return core::ImageCompareFn::NotEqual;
        case GL_GEQUAL: return core::ImageCompareFn::GreaterEqual;
        default: UNREACHABLE;
    }
}
/**
 * @brief Maps Siren image dimensions and extent to OpenGL texture targets.
 * Works as follows:
 * - @b D1: Returns @c GL_TEXTURE_1D or @c GL_TEXTURE_1D_ARRAY.
 * - @b D2: Returns @c GL_TEXTURE_2D or @c GL_TEXTURE_2D_ARRAY.
 * - @b D3: Always returns @c GL_TEXTURE_3D (3D arrays are not supported in OpenGL).
 * - @b Cube: Returns @c GL_TEXTURE_CUBE_MAP (iff 6 layers) or @c GL_TEXTURE_CUBE_MAP_ARRAY.
 *
 * @param extent The @ref core::ImageExtent.
 * @param dimension The @ref core::ImageDimension.
 * @return GLenum The resulting OpenGL texture target (e.g., @c GL_TEXTURE_2D_ARRAY).
 */
constexpr auto img_to_target_gl(const core::ImageExtent extent, const core::ImageDimension dimension) -> GLenum {
    switch (dimension) {
        case core::ImageDimension::D1: return (extent.depth_or_layers > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
        case core::ImageDimension::D2: return (extent.depth_or_layers > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
        // There are no 3D arrays in GL.
        case core::ImageDimension::D3: return GL_TEXTURE_3D;
        // 6 layers = 1 cube. > 6 layers = Array of cubes.
        case core::ImageDimension::Cube: return (extent.depth_or_layers > 6)
                                                    ? GL_TEXTURE_CUBE_MAP_ARRAY
                                                    : GL_TEXTURE_CUBE_MAP;
        default: SIREN_ASSERT(false, "Unknown ImageDimension");
    }
}

/**
 * @brief Converts enum BufferUsage to native OpenGL storage flags.
 *
 * - @b Static: Returns @c 0. This creates immutable, non CPU accessible storage.
 * Updates must be performed via staging buffers.
 * - @b Dynamic: Returns @c GL_DYNAMIC_STORAGE_BIT. Enables @c glNamedBufferSubData
 * for occasional CPU-to-GPU updates.
 * - @b Stream: Returns a combination of @c GL_DYNAMIC_STORAGE_BIT, @c GL_MAP_WRITE_BIT,
 * @c GL_MAP_PERSISTENT_BIT, and @c GL_MAP_COHERENT_BIT. This enables "Persistent Mapping,"
 * allowing the CPU to write directly to a GPU pointer without unmapping or explicit
 * flushing (No @c glFlushMappedNamedBufferRange required).
 *
 * @note Reading from GPU memory is explicitly unsupported. If readback is required,
 * the caller should maintain a CPU copy.
 *
 * @param usage The @ref core::BufferUsage for the buffer.
 * @return GLbitfield The bitmask of OpenGL storage flags.
 */
constexpr auto buffer_usage_to_flags_gl(const core::BufferUsage usage) -> GLbitfield {
    if (usage == core::BufferUsage::Dynamic) { return GL_DYNAMIC_STORAGE_BIT; }
    if (usage == core::BufferUsage::Stream) {
        return GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    }
    return 0;
}

/**
 * @brief Converts a siren @ref ShaderStage to its native GLenum version.
 * @param shader_stage The @ref ShaderStage to convert.
 * @return A converted GLenum.
 */
constexpr auto shader_stage_to_gl(const core::ShaderStage shader_stage) -> GLenum {
    switch (shader_stage) {
        case core::ShaderStage::Vertex: return GL_VERTEX_SHADER;
        case core::ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
        case core::ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
        case core::ShaderStage::Compute: return GL_COMPUTE_SHADER;
        case core::ShaderStage::Task:
        case core::ShaderStage::Mesh:
        default: SIREN_ASSERT(false, "Unsupported Shader stage for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref DepthFunction to its native GLenum version.
 * @param depth_function The @ref DepthFunction to convert
 * @return A converted GLenum.
 */
constexpr auto depth_func_to_gl(const core::DepthFunction depth_function) -> GLenum {
    switch (depth_function) {
        case core::DepthFunction::Always: return GL_ALWAYS;
        case core::DepthFunction::Never: return GL_NEVER;
        case core::DepthFunction::Less: return GL_LESS;
        case core::DepthFunction::Equal: return GL_EQUAL;
        case core::DepthFunction::LessEqual: return GL_LEQUAL;
        case core::DepthFunction::Greater: return GL_GREATER;
        case core::DepthFunction::GreaterEqual: return GL_GEQUAL;
        case core::DepthFunction::NotEqual: return GL_NOTEQUAL;
        default: SIREN_ASSERT(false, "Unsupported Depth Function for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref PrimitiveTopology to its native GLenum version.
 * @param topology The @ref PrimitiveTopology to convert.
 * @return A converted GLenum.
 */
constexpr auto topology_to_gl(const core::PrimitiveTopology topology) -> GLenum {
    switch (topology) {
        case core::PrimitiveTopology::Points: return GL_POINTS;
        case core::PrimitiveTopology::Lines: return GL_LINES;
        case core::PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
        case core::PrimitiveTopology::Triangles: return GL_TRIANGLES;
        case core::PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
        case core::PrimitiveTopology::TriangleFan: return GL_TRIANGLE_FAN;
        default: SIREN_ASSERT(false, "Unsupported PrimitiveTopology for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref IndexFormat to its native GLenum version.
 * @param format The @ref IndexFormat to convert.
 * @return A converted GLenum.
 */
constexpr auto index_format_to_gl(const core::IndexFormat format) -> GLenum {
    switch (format) {
        case core::IndexFormat::Byte8: return GL_UNSIGNED_BYTE;
        case core::IndexFormat::Short16: return GL_UNSIGNED_SHORT;
        case core::IndexFormat::Uint32: return GL_UNSIGNED_INT;
        default: SIREN_ASSERT(false, "Unsupported IndexFormat for OpenGL Backend");
    }
}

} // namespace siren::platform::gl
