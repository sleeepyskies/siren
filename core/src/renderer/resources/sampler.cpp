#include "sampler.hpp"

#include "renderer/device.hpp"


namespace siren::core
{

Sampler::Sampler(
    Device* device,
    const SamplerHandle handle,
    const SamplerDescriptor& descriptor
) : Base(device, handle),
    m_descriptor(descriptor) { }

Sampler::~Sampler() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_sampler(m_handle);
    }
}

Sampler::Sampler(Sampler&& other) noexcept
    : Base(std::move(other)),
      m_descriptor(std::move(other.m_descriptor)) { }

Sampler& Sampler::operator=(Sampler&& other) noexcept {
    if (this != &other) {
        // cleanup old sampler
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_sampler(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);
    }
    return *this;
}

} // namespace siren::core
