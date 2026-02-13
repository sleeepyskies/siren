#include "open_gl_device.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "mappings.hpp"

#include "platform/GL.hpp"
#include "sync/render_thread.hpp"


namespace siren::platform
{

static auto render_thread() -> core::RenderThread& { return core::Locator<core::RenderThread>::locate(); }

OpenGlDevice::OpenGlDevice() : Device() { }

OpenGlDevice::~OpenGlDevice() { }

auto OpenGlDevice::create_buffer(const core::BufferDescriptor& descriptor) -> core::Buffer {
    SIREN_ASSERT(descriptor.size > 0, "Cannot allocate empty buffer.");
    auto buffer_handle = m_buffer_table.reserve();
    render_thread().spawn(
        [buffer_handle, descriptor, this] {
            GLuint buf;
            glCreateBuffers(1, &buf);
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_BUFFER, buf, descriptor.label.value().size(), descriptor.label.value().data());
            }

            this->m_buffer_table.link(buffer_handle, buf);
        }
    );
    return core::Buffer{ this, buffer_handle, descriptor };
}

auto OpenGlDevice::destroy_buffer(const core::BufferHandle handle) -> void {
    // opengl ignores 0 values, so we don't need to check
    const auto api_handle = m_buffer_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Buffer });
}

auto OpenGlDevice::create_image(const core::ImageDescriptor& descriptor) -> core::Image {
    SIREN_ASSERT(descriptor.img_data.size() > 0, "Cannot create an empty image.");
    SIREN_ASSERT(
        descriptor.extent.width > 0 || descriptor.extent.height > 0 || descriptor.extent.depth_or_layers > 0,
        "Cannot create an empty image."
    );

    TODO;

    return core::Image{ this, image_handle, };
}

auto OpenGlDevice::destroy_image(const core::ImageHandle handle) -> void {
    const auto api_handle = m_image_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Image });
}

auto OpenGlDevice::create_sampler(const core::SamplerDescriptor& descriptor) -> core::Sampler {
    const auto sampler_handle = m_sampler_table.reserve();

    render_thread().spawn(
        [descriptor, sampler_handle, this]() {
            GLuint sampler;
            glGenSamplers(1, &sampler);
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_MIN_FILTER,
                gl::min_img_filter_to_gl(descriptor.min_filter, descriptor.mipmap_filter)
            );
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_MAG_FILTER,
                gl::img_filter_to_gl(descriptor.min_filter)
            );

            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, gl::img_wrap_to_gl(descriptor.s_wrap));
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, gl::img_wrap_to_gl(descriptor.t_wrap));
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, gl::img_wrap_to_gl(descriptor.r_wrap));

            glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, descriptor.lod_min);
            glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, descriptor.lod_max);

            if (descriptor.border_color.has_value()) {
                glSamplerParameterfv(
                    sampler,
                    GL_TEXTURE_BORDER_COLOR,
                    reinterpret_cast<const GLfloat*>(glm::value_ptr(descriptor.border_color.value()))
                );
            }

            glSamplerParameteri(
                sampler,
                GL_TEXTURE_COMPARE_MODE,
                gl::img_compare_mode_to_gl(descriptor.compare_mode)
            );
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_COMPARE_FUNC,
                gl::img_compare_fn_to_gl(descriptor.compare_fn)
            );

            this->m_sampler_table.link(sampler_handle, sampler);
        }
    );

    return core::Sampler{ this, sampler_handle, descriptor };
}

auto OpenGlDevice::destroy_sampler(const core::SamplerHandle handle) -> void {
    const auto api_handle = m_sampler_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
}

} // namespace siren::platform
