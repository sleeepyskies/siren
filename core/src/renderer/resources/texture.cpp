#include "texture.hpp"

#include "platform/opengl/mappings.hpp"
#include "renderer/device.hpp"


namespace siren::core
{

Image::Image(
    Device* device,
    const ImageHandle handle,
    const ImageFormat& image_format,
    const ImageExtent& image_extent,
    const ImageDimension& image_dimension,
    const u32 mipmap_levels
) : Base(device, handle),
    m_format(image_format),
    m_extent(image_extent),
    m_dimension(image_dimension),
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

Image::Image(Image&& other) noexcept
    : RenderResource(std::move(other)),
      m_format(other.m_format),
      m_extent(other.m_extent),
      m_dimension(other.m_dimension),
      m_mipmap_levels(other.m_mipmap_levels) { other.m_handle = { }; }

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
