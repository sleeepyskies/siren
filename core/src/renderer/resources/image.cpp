#include "image.hpp"

#include "renderer/device.hpp"


namespace siren::core
{

Image::Image(
    Device* device,
    const ImageHandle handle,
    const ImageDescriptor& descriptor
) : Base(device, handle),
    m_descriptor(descriptor) { }

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

} // namespace siren::core
