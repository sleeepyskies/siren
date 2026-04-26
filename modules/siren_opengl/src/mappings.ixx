module;

#include <libassert/assert.hpp>
#include <glad/gl.h>

export module siren.opengl:mappings;

import siren.render;

export namespace siren::render {
constexpr auto img_filter_to_gl(const ImageFilterMode mode) -> GLenum {
    switch (mode) {
        case ImageFilterMode::Nearest: return GL_NEAREST;
        case ImageFilterMode::Linear: return GL_LINEAR;
        default: UNREACHABLE();
    }
}

// OpenGL is weird and chose to combine lod filtering and min filtering.

constexpr auto min_img_filter_to_gl(const ImageFilterMode min, const ImageFilterMode lod) -> GLenum {
    if (min == ImageFilterMode::Linear) {
        return lod == ImageFilterMode::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
    }
    return lod == ImageFilterMode::Linear ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
}

constexpr auto img_filter_to_siren(const GLenum mode) -> ImageFilterMode {
    switch (mode) {
        case GL_NEAREST: return ImageFilterMode::Nearest;
        case GL_LINEAR: return ImageFilterMode::Linear;
        default: UNREACHABLE();
    }
}

constexpr auto img_wrap_to_gl(const ImageWrapMode mode) -> GLenum {
    switch (mode) {
        case ImageWrapMode::Repeat: return GL_REPEAT;
        case ImageWrapMode::Mirror: return GL_MIRRORED_REPEAT;
        case ImageWrapMode::ClampEdge: return GL_CLAMP_TO_EDGE;
        case ImageWrapMode::ClampBorder: return GL_CLAMP_TO_BORDER;
        default: UNREACHABLE();
    }
}
constexpr auto img_wrap_to_siren(const GLenum mode) -> ImageWrapMode {
    switch (mode) {
        case GL_REPEAT: return ImageWrapMode::Repeat;
        case GL_MIRRORED_REPEAT: return ImageWrapMode::Mirror;
        case GL_CLAMP_TO_EDGE: return ImageWrapMode::ClampEdge;
        case GL_CLAMP_TO_BORDER: return ImageWrapMode::ClampBorder;
        default: UNREACHABLE();
    }
}

constexpr auto img_dim_to_gl(const ImageDimension dim) -> GLenum {
    switch (dim) {
        case ImageDimension::D1: return GL_TEXTURE_1D;
        case ImageDimension::D2: return GL_TEXTURE_2D;
        case ImageDimension::D3: return GL_TEXTURE_3D;
        default: UNREACHABLE();
    }
}

constexpr auto img_dim_to_siren(const GLenum dim) -> ImageDimension {
    switch (dim) {
        case GL_TEXTURE_1D: return ImageDimension::D1;
        case GL_TEXTURE_2D: return ImageDimension::D2;
        case GL_TEXTURE_3D: return ImageDimension::D3;
        default: UNREACHABLE();
    }
}

// This describes the gpu vram layout

constexpr auto img_format_to_gl_internal(const ImageFormat format) -> GLenum {
    switch (format) {
        case ImageFormat::Mask8: return GL_R8;
        case ImageFormat::LinearColor8: return GL_RGBA8;
        case ImageFormat::Color8: return GL_SRGB8_ALPHA8;
        case ImageFormat::Hdr16: return GL_RGB16F;
        case ImageFormat::DepthStencil: return GL_DEPTH24_STENCIL8;
        default: UNREACHABLE();
    }
}

// This describes the cpu side layout

constexpr auto img_format_to_gl_layout(const ImageFormat format) -> GLenum {
    switch (format) {
        case ImageFormat::Mask8: return GL_RED;
        case ImageFormat::LinearColor8:
        case ImageFormat::Color8: return GL_RGBA;
        case ImageFormat::Hdr16: return GL_RGB;
        case ImageFormat::DepthStencil: return GL_DEPTH_STENCIL;
        default: UNREACHABLE();
    }
}

constexpr auto img_format_from_gl_internal(const GLenum internal_format) -> ImageFormat {
    switch (internal_format) {
        case GL_R8: return ImageFormat::Mask8;
        case GL_RGBA8: return ImageFormat::LinearColor8;
        case GL_SRGB8_ALPHA8: return ImageFormat::Color8;
        case GL_RGB16F: return ImageFormat::Hdr16;
        case GL_DEPTH24_STENCIL8: return ImageFormat::DepthStencil;
        default: return ImageFormat::Unknown;
    }
}

/**
 * @brief Converts a siren @ref ImageCompareMode to its native GLint version.
 * @param mode The @ref ImageCompareMode to convert.
 * @return A converted GLint.
 */
constexpr auto img_compare_mode_to_gl(const ImageCompareMode mode) -> GLint {
    switch (mode) {
        case ImageCompareMode::None: return GL_NONE;
        case ImageCompareMode::Compare: return GL_COMPARE_REF_TO_TEXTURE;
        default: UNREACHABLE();
    }
}

/**
 * @brief Converts an OpenGL GLint to a siren ImageCompareMode.
 * @param mode The GLint to convert.
 * @return A converted siren @ref ImageCompareMode.
 */
constexpr auto img_compare_mode_to_siren(const GLint mode) -> ImageCompareMode {
    switch (mode) {
        case GL_NONE: return ImageCompareMode::None;
        case GL_COMPARE_REF_TO_TEXTURE: return ImageCompareMode::Compare;
        default: UNREACHABLE();
    }
}

/**
 * @brief Converts a siren @ref ImageCompareFn to its native GLenum version.
 * @param func The @ref ImageCompareFn to convert.
 * @return A converted GLenum.
 */
constexpr auto img_compare_fn_to_gl(const ImageCompareFn func) -> GLenum {
    switch (func) {
        case ImageCompareFn::Always: return GL_ALWAYS;
        case ImageCompareFn::Never: return GL_NEVER;
        case ImageCompareFn::Less: return GL_LESS;
        case ImageCompareFn::Equal: return GL_EQUAL;
        case ImageCompareFn::LessEqual: return GL_LEQUAL;
        case ImageCompareFn::Greater: return GL_GREATER;
        case ImageCompareFn::NotEqual: return GL_NOTEQUAL;
        case ImageCompareFn::GreaterEqual: return GL_GEQUAL;
        default: UNREACHABLE();
    }
}

/**
 * @brief Converts an OpenGL GLenum to a siren ImageCompareFn.
 * @param func The GLenum to convert.
 * @return A converted siren @ref ImageCompareFn.
 */
constexpr auto img_compare_fn_to_siren(const GLenum func) -> ImageCompareFn {
    switch (func) {
        case GL_ALWAYS: return ImageCompareFn::Always;
        case GL_NEVER: return ImageCompareFn::Never;
        case GL_LESS: return ImageCompareFn::Less;
        case GL_EQUAL: return ImageCompareFn::Equal;
        case GL_LEQUAL: return ImageCompareFn::LessEqual;
        case GL_GREATER: return ImageCompareFn::Greater;
        case GL_NOTEQUAL: return ImageCompareFn::NotEqual;
        case GL_GEQUAL: return ImageCompareFn::GreaterEqual;
        default: UNREACHABLE();
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
 * @param extent The @ref ImageExtent.
 * @param dimension The @ref ImageDimension.
 * @return GLenum The resulting OpenGL texture target (e.g., @c GL_TEXTURE_2D_ARRAY).
 */
constexpr auto img_to_target_gl(const ImageExtent extent, const ImageDimension dimension) -> GLenum {
    switch (dimension) {
        case ImageDimension::D1: return (extent.depth_or_layers > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
        case ImageDimension::D2: return (extent.depth_or_layers > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
        // There are no 3D arrays in GL.
        case ImageDimension::D3: return GL_TEXTURE_3D;
        // 6 layers = 1 cube. > 6 layers = Array of cubes.
        case ImageDimension::Cube: return (extent.depth_or_layers > 6)
                                              ? GL_TEXTURE_CUBE_MAP_ARRAY
                                              : GL_TEXTURE_CUBE_MAP;
        default: UNREACHABLE("Unknown ImageDimension");
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
 * @param usage The @ref BufferUsage for the buffer.
 * @return GLbitfield The bitmask of OpenGL storage flags.
 */
constexpr auto buffer_usage_to_flags_gl(const BufferUsage usage) -> GLbitfield {
    if (usage == BufferUsage::Dynamic) { return GL_DYNAMIC_STORAGE_BIT; }
    if (usage == BufferUsage::Stream) {
        return GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    }
    return 0;
}

/**
 * @brief Converts a siren @ref ShaderStage to its native GLenum version.
 * @param shader_stage The @ref ShaderStage to convert.
 * @return A converted GLenum.
 */
constexpr auto shader_stage_to_gl(const ShaderStage shader_stage) -> GLenum {
    switch (shader_stage) {
        case ShaderStage::Vertex: return GL_VERTEX_SHADER;
        case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderStage::Compute: return GL_COMPUTE_SHADER;
        case ShaderStage::Task:
        case ShaderStage::Mesh:
        default: UNREACHABLE("Unsupported Shader stage for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref DepthFunction to its native GLenum version.
 * @param depth_function The @ref DepthFunction to convert
 * @return A converted GLenum.
 */
constexpr auto depth_func_to_gl(const DepthFunction depth_function) -> GLenum {
    switch (depth_function) {
        case DepthFunction::Always: return GL_ALWAYS;
        case DepthFunction::Never: return GL_NEVER;
        case DepthFunction::Less: return GL_LESS;
        case DepthFunction::Equal: return GL_EQUAL;
        case DepthFunction::LessEqual: return GL_LEQUAL;
        case DepthFunction::Greater: return GL_GREATER;
        case DepthFunction::GreaterEqual: return GL_GEQUAL;
        case DepthFunction::NotEqual: return GL_NOTEQUAL;
        default: UNREACHABLE("Unsupported Depth Function for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref PrimitiveTopology to its native GLenum version.
 * @param topology The @ref PrimitiveTopology to convert.
 * @return A converted GLenum.
 */
constexpr auto topology_to_gl(const PrimitiveTopology topology) -> GLenum {
    switch (topology) {
        case PrimitiveTopology::Points: return GL_POINTS;
        case PrimitiveTopology::Lines: return GL_LINES;
        case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
        case PrimitiveTopology::Triangles: return GL_TRIANGLES;
        case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan: return GL_TRIANGLE_FAN;
        default: UNREACHABLE(false, "Unsupported PrimitiveTopology for OpenGL Backend");
    }
}

/**
 * @brief Converts a siren @ref IndexFormat to its native GLenum version.
 * @param format The @ref IndexFormat to convert.
 * @return A converted GLenum.
 */
constexpr auto index_format_to_gl(const IndexFormat format) -> GLenum {
    switch (format) {
        case IndexFormat::Byte8: return GL_UNSIGNED_BYTE;
        case IndexFormat::Short16: return GL_UNSIGNED_SHORT;
        case IndexFormat::Uint32: return GL_UNSIGNED_INT;
        default: UNREACHABLE("Unsupported IndexFormat for OpenGL Backend");
    }
}
}

