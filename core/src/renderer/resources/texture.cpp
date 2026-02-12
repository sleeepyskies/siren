#include "texture.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "platform/opengl/mappings.hpp"


namespace siren::core
{
Sampler::Sampler(
    SamplerDescription&& desc
) : m_handle(0), m_desc(std::forward<SamplerDescription>(desc)) {
    glGenSamplers(1, &m_handle.value);
    glSamplerParameteri(
        m_handle.value,
        GL_TEXTURE_MIN_FILTER,
        platform::gl::min_img_filter_to_gl(desc.min_filter, desc.mipmap_filter)
    );
    glSamplerParameteri(m_handle.value, GL_TEXTURE_MAG_FILTER, platform::gl::img_filter_to_gl(desc.min_filter));

    glSamplerParameteri(m_handle.value, GL_TEXTURE_WRAP_S, platform::gl::img_wrap_to_gl(desc.s_wrap));
    glSamplerParameteri(m_handle.value, GL_TEXTURE_WRAP_T, platform::gl::img_wrap_to_gl(desc.t_wrap));
    glSamplerParameteri(m_handle.value, GL_TEXTURE_WRAP_R, platform::gl::img_wrap_to_gl(desc.r_wrap));

    glSamplerParameterf(m_handle.value, GL_TEXTURE_MIN_LOD, desc.lod_min);
    glSamplerParameterf(m_handle.value, GL_TEXTURE_MAX_LOD, desc.lod_max);

    if (desc.border_color.has_value()) {
        glSamplerParameterfv(
            m_handle.value,
            GL_TEXTURE_BORDER_COLOR,
            reinterpret_cast<const GLfloat*>(glm::value_ptr(desc.border_color.value()))
        );
    }

    glSamplerParameteri(
        m_handle.value,
        GL_TEXTURE_COMPARE_MODE,
        platform::gl::img_compare_mode_to_gl(desc.compare_mode)
    );
    glSamplerParameteri(m_handle.value, GL_TEXTURE_COMPARE_FUNC, platform::gl::img_compare_fn_to_gl(desc.compare_fn));
}

Sampler::~Sampler() {
    glDeleteSamplers(1, &m_handle.value);
}
Sampler& Sampler::operator=(Sampler&& other) noexcept {
    m_handle       = other.m_handle;
    m_desc         = std::move(other.m_desc);
    other.m_handle = { 0 };
    return *this;
}

Image::Image(
    const std::span<const u8> data,
    const ImageFormat& image_format,
    const ImageExtent& image_extent,
    const ImageDimension& image_dimension,
    const u32 mipmap_levels
) : m_handle(0), m_format(image_format), m_extent(image_extent), m_dimension(image_dimension),
    m_mipmap_levels(mipmap_levels) {
    // create Image handle
    glCreateTextures(platform::gl::img_to_target_gl(image_extent, image_dimension), 1, &m_handle.value);

    // allocate enough memory
    switch (m_dimension) {
        case ImageDimension::D1: {
            if (m_extent.depth_or_layers == 1) {
                glTextureStorage1D(
                    m_handle.value,
                    m_mipmap_levels,
                    platform::gl::img_format_to_gl_internal(m_format),
                    m_extent.width
                );
            } else {
                glTextureStorage2D(
                    m_handle.value,
                    m_mipmap_levels,
                    platform::gl::img_format_to_gl_internal(m_format),
                    m_extent.width,
                    m_extent.depth_or_layers
                );
            }
            break;
        }
        case ImageDimension::D2: {
            if (m_extent.depth_or_layers == 1) {
                glTextureStorage2D(
                    m_handle.value,
                    m_mipmap_levels,
                    platform::gl::img_format_to_gl_internal(m_format),
                    m_extent.width,
                    m_extent.height
                );
            } else {
                glTextureStorage3D(
                    m_handle.value,
                    m_mipmap_levels,
                    platform::gl::img_format_to_gl_internal(m_format),
                    m_extent.width,
                    m_extent.height,
                    m_extent.depth_or_layers
                );
            }
            break;
        };
        case ImageDimension::D3: {
            glTextureStorage3D(
                m_handle.value,
                m_mipmap_levels,
                platform::gl::img_format_to_gl_internal(m_format),
                m_extent.width,
                m_extent.height,
                m_extent.depth_or_layers
            );
            break;
        };
        case ImageDimension::Cube: {
            glTextureStorage2D(
                m_handle.value,
                m_mipmap_levels,
                platform::gl::img_format_to_gl_internal(m_format),
                m_extent.width,
                m_extent.height
            );
            break;
        };
        default: UNREACHABLE;
    }

    // upload data to gpu
    if (!data.empty()) {
        // just upload it all in one go, this should be fine even for cube maps
        glTextureSubImage3D(
            m_handle.value,
            0,
            0,
            0,
            0,
            m_extent.width,
            m_extent.height,
            m_extent.depth_or_layers,
            platform::gl::img_format_to_gl_layout(m_format),
            GL_UNSIGNED_BYTE,
            data.data()
        );
    }

    // generate mip map levels
    if (m_mipmap_levels > 1) {
        glGenerateTextureMipmap(m_handle.value);
    }
}

Image::~Image() {
    glDeleteTextures(1, &m_handle.value);
}

Image& Image::operator=(Image&& other) noexcept {
    m_handle        = other.m_handle;
    m_format        = other.m_format;
    m_extent        = other.m_extent;
    m_dimension     = other.m_dimension;
    m_mipmap_levels = other.m_mipmap_levels;
    other.m_handle  = { 0 };
    return *this;
}
} // namespace siren::core
