/// @file mappings.hpp
/// @brief Contains all Siren->OpenGL and OpenGL->Siren mappings.
#pragma once

#include "platform/GL.hpp"
#include "../../renderer/resources/Texture.hpp"


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

constexpr auto img_compare_mode_to_gl(const core::ImageCompareMode mode) -> GLint {
    switch (mode) {
        case core::ImageCompareMode::None: return GL_NONE;
        case core::ImageCompareMode::Compare: return GL_COMPARE_REF_TO_TEXTURE;
        default: UNREACHABLE;
    }
}

constexpr auto img_compare_mode_to_siren(const GLint mode) -> core::ImageCompareMode {
    switch (mode) {
        case GL_NONE: return core::ImageCompareMode::None;
        case GL_COMPARE_REF_TO_TEXTURE: return core::ImageCompareMode::Compare;
        default: UNREACHABLE;
    }
}

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

constexpr auto img_to_target_gl(const core::ImageExtent extent, const core::ImageDimension dimension) -> GLenum {
    switch (dimension) {
        case core::ImageDimension::D1: {
            return extent.depth_or_layers > 1 ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
        }
        case core::ImageDimension::D2: {
            return extent.depth_or_layers > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
        }
        case core::ImageDimension::D3: {
            if (extent.depth_or_layers == 1) { return GL_TEXTURE_3D; }
        };
        case core::ImageDimension::Cube: {
            return extent.depth_or_layers > 6 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_CUBE_MAP_ARRAY;
        };
        default: UNREACHABLE;
    };
}

} // namespace siren::platform::gl
