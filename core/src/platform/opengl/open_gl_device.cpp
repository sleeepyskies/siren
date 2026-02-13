#include "open_gl_device.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "mappings.hpp"

#include "platform/GL.hpp"
#include "sync/render_thread.hpp"


namespace siren::platform
{

using namespace siren::core;

/// helper method to reduce code. just fetches the render thread from the locator.
static auto render_thread() -> RenderThread& { return Locator<RenderThread>::locate(); }

OpenGlDevice::OpenGlDevice() : Device() { }

OpenGlDevice::~OpenGlDevice() { }

auto OpenGlDevice::create_buffer(const BufferDescriptor& descriptor) -> Buffer {
    SIREN_ASSERT(descriptor.size > 0, "Cannot allocate empty buffer.");
    const auto buffer_handle = m_buffer_table.reserve();
    render_thread().spawn(
        [buffer_handle, descriptor, this] {
            // create buffer id
            GLuint buf;
            glCreateBuffers(1, &buf);

            // optionally name it
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_BUFFER, buf, descriptor.label.value().size(), descriptor.label.value().data());
            }

            const auto flags = gl::buffer_usage_to_flags_gl(descriptor.usage);

            // allocate storage for it, but don't upload yet
            glNamedBufferStorage(buf, descriptor.size, nullptr, flags);

            // if the buffer is streamed, we also need to store a mapping pointer
            void* mapped_ptr = nullptr;
            if (descriptor.usage == BufferUsage::Stream) {
                mapped_ptr = glMapNamedBufferRange(
                    buf,
                    0,
                    descriptor.size,
                    GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT
                );
            }

            // link proxy handle to opengl handle
            this->m_buffer_table.link(buffer_handle, buf, mapped_ptr);
        }
    );

    return Buffer{ this, buffer_handle, descriptor };
}

auto OpenGlDevice::destroy_buffer(const BufferHandle handle) -> void {
    // opengl ignores 0 values, so we don't need to check
    const auto api_handle = m_buffer_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Buffer });
}

auto OpenGlDevice::create_image(const ImageDescriptor& descriptor) -> Image {
    SIREN_ASSERT(
        descriptor.extent.width > 0 || descriptor.extent.height > 0 || descriptor.extent.depth_or_layers > 0,
        "Cannot create an empty image."
    );
    const auto image_handle = m_image_table.reserve();
    render_thread().spawn(
        [image_handle, descriptor, this] {
            // create the image
            GLuint img;
            glCreateTextures(gl::img_to_target_gl(descriptor.extent, descriptor.dimension), 1, &img);

            // assign the proxy handle to the real handle
            this->m_image_table.link(image_handle, img);
        }
    );

    return Image{ this, image_handle, descriptor };
}

auto OpenGlDevice::destroy_image(const ImageHandle handle) -> void {
    const auto api_handle = m_image_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Image });
}

auto OpenGlDevice::create_sampler(const SamplerDescriptor& descriptor) -> Sampler {
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

    return Sampler{ this, sampler_handle, descriptor };
}

auto OpenGlDevice::destroy_sampler(const SamplerHandle handle) -> void {
    const auto api_handle = m_sampler_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
}

} // namespace siren::platform
