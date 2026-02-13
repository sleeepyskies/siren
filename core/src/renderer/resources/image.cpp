#include "image.hpp"

#include "platform/opengl/mappings.hpp"
#include "renderer/device.hpp"


namespace siren::core
{

Image::Image(
    Device* device,
    const ImageHandle handle,
    const ImageDescriptor& descriptor
) : Base(device, handle),
    m_descriptor(descriptor) {
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
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_image(m_handle);
    }
}

Image::Image(Image&& other) noexcept
    : Base(std::move(other)),
      m_descriptor(std::move(other.descriptor())) { }

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_image(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);
    }
    return *this;
}

auto Image::descriptor() const noexcept -> const ImageDescriptor& { return m_descriptor; }

auto Image::upload(std::span<const u8> data) const noexcept -> std::expected<void, Error> {
    if (!m_device) {
        return std::unexpected(Error{ Code::DeviceNotPresent });
    }

    auto recorder = m_device->record_commands();
    recorder->upload_to_buffer(m_handle, data);
    m_device->submit(std::move(recorder));

    return { };
}

} // namespace siren::core
