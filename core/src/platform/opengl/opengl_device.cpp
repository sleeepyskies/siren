#include "opengl_device.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "opengl_command_buffer.hpp"
#include "mappings.hpp"
#include "platform/gl.hpp"
#include "sync/render_thread.hpp"


namespace siren::platform
{

// todo: add logging

using namespace siren::core;

/// helper method to reduce code. just fetches the render thread from the locator.
static constexpr auto render_thread() -> RenderThread& { return Locator<RenderThread>::locate(); }

OpenGLDevice::OpenGLDevice() : Device() {
    m_logger = Locator<Logger>::locate().renderer;
}

OpenGLDevice::~OpenGLDevice() { }

auto OpenGLDevice::create_buffer(const BufferDescriptor& descriptor) -> Buffer {
    SIREN_ASSERT(descriptor.size > 0, "Cannot legally allocate empty buffer (sorry).");
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

auto OpenGLDevice::destroy_buffer(const BufferHandle handle) -> void {
    // opengl ignores 0 values, so we don't need to check
    const auto api_handle = m_buffer_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Buffer });
}

auto OpenGLDevice::create_image(const ImageDescriptor& descriptor) -> Image {
    SIREN_ASSERT(
        descriptor.extent.width > 0 || descriptor.extent.height > 0 || descriptor.extent.depth_or_layers > 0,
        "Cannot create an empty image."
    );
    const auto image_handle = m_image_table.reserve();
    render_thread().spawn(
        [image_handle, descriptor, this] {
            const auto target = gl::img_to_target_gl(descriptor.extent, descriptor.dimension);

            // create the image
            GLuint img;
            glCreateTextures(target, 1, &img);

            // optionally name it
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_TEXTURE, img, descriptor.label.value().size(), descriptor.label.value().data());
            }

            const auto internal_format = gl::img_format_to_gl_internal(descriptor.format);
            const auto& ext            = descriptor.extent;

            // allocate enough memory
            switch (target) {
                case GL_TEXTURE_1D:
                    glTextureStorage1D(img, descriptor.mipmap_levels, internal_format, ext.width);
                    break;
                case GL_TEXTURE_1D_ARRAY:
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP: // Cubemaps use 2D storage
                    glTextureStorage2D(img, descriptor.mipmap_levels, internal_format, ext.width, ext.height);
                    break;
                case GL_TEXTURE_2D_ARRAY:
                case GL_TEXTURE_3D:
                case GL_TEXTURE_CUBE_MAP_ARRAY:
                    glTextureStorage3D(
                        img,
                        descriptor.mipmap_levels,
                        internal_format,
                        ext.width,
                        ext.height,
                        ext.depth_or_layers
                    );
                    break;
                default: SIREN_ASSERT(false, "Unsupported texture target");
            }

            // assign the proxy handle to the real handle
            this->m_image_table.link(image_handle, img);
        }
    );

    return Image{ this, image_handle, descriptor };
}

auto OpenGLDevice::destroy_image(const ImageHandle handle) -> void {
    const auto api_handle = m_image_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Image });
}

auto OpenGLDevice::create_sampler(const SamplerDescriptor& descriptor) -> Sampler {
    const auto sampler_handle = m_sampler_table.reserve();

    render_thread().spawn(
        [descriptor, sampler_handle, this]() {
            GLuint sampler;
            glCreateSamplers(1, &sampler);
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

auto OpenGLDevice::destroy_sampler(const SamplerHandle handle) -> void {
    const auto api_handle = m_sampler_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
}

auto OpenGLDevice::create_framebuffer(const FramebufferDescriptor& descriptor) -> Framebuffer {
    SIREN_ASSERT(descriptor.width > 0, "Framebuffer must have a width of at least 1 pixel.");
    SIREN_ASSERT(descriptor.height > 0, "Framebuffer must have a height of at least 1 pixel.");
    SIREN_ASSERT(
        descriptor.has_color || descriptor.has_depth || descriptor.has_stencil,
        "Framebuffer must have at least one attachment."
    );

    // helper to create an image label
    auto make_label = [] (
        const std::optional<std::string>& fb_label, const std::string_view img_label
    ) -> std::optional<std::string> {
        if (fb_label) {
            return *fb_label + "-" + std::string(img_label);
        }
        return std::nullopt;
    };

    const auto fb_handle = m_framebuffer_table.reserve();

    // internally, RenderThread is sequential. So we first request to create Images,
    // which are guaranteed to exist by the time we  create the Framebuffer.

    // helper struct to avoid ptr to opt conversion
    struct Attachment {
        std::optional<Image> img = std::nullopt;
        auto handle() const noexcept -> std::optional<ImageHandle> {
            if (img.has_value()) { return std::nullopt; }
            return img->handle();
        }
    };

    Attachment color;
    Attachment depth;
    Attachment stencil;

    if (descriptor.has_color) {
        color.img = this->create_image(
            {
                .label = make_label(descriptor.label, "Color Attachment"),
                .format = ImageFormat::LinearColor8,
                .extent = { .width = descriptor.width, .height = descriptor.height },
                .dimension = ImageDimension::D2,
                .mipmap_levels = 0
            }
        );
    }

    if (descriptor.has_depth) {
        depth.img = this->create_image(
            {
                .label = make_label(descriptor.label, "Depth Attachment"),
                .format = ImageFormat::DepthStencil,
                .extent = { .width = descriptor.width, .height = descriptor.height },
                .dimension = ImageDimension::D2,
                .mipmap_levels = 0
            }
        );
    }

    if (descriptor.has_depth) {
        depth.img = this->create_image(
            {
                .label = make_label(descriptor.label, "Stencil Attachment"),
                .format = ImageFormat::DepthStencil,
                .extent = { .width = descriptor.width, .height = descriptor.height },
                .dimension = ImageDimension::D2,
                .mipmap_levels = 0
            }
        );
    }

    render_thread().spawn(
        [
            fb_handle,
            descriptor,
            color_handle = color.handle(),
            depth_handle = depth.handle(),
            stencil_handle = stencil.handle(),
            this
        ] {
            GLuint framebuffer;
            glCreateFramebuffers(1, &framebuffer);

            // optionally label the framebuffer
            if (descriptor.label.has_value()) {
                glObjectLabel(
                    GL_FRAMEBUFFER,
                    framebuffer,
                    descriptor.label.value().size(),
                    descriptor.label.value().c_str()
                );
            }

            // setup color attachment
            if (descriptor.has_color && color_handle.has_value()) {
                glNamedFramebufferTexture(
                    framebuffer,
                    GL_COLOR_ATTACHMENT0,
                    this->m_image_table.fetch(color_handle.value()),
                    0
                );
            }

            // setup depth attachment
            if (descriptor.has_depth && depth_handle.has_value()) {
                glNamedFramebufferTexture(
                    framebuffer,
                    GL_DEPTH_ATTACHMENT,
                    this->m_image_table.fetch(depth_handle.value()),
                    0
                );
            }

            // setup stencil attachment
            if (descriptor.has_stencil && stencil_handle.has_value()) {
                glNamedFramebufferTexture(
                    framebuffer,
                    GL_STENCIL_ATTACHMENT,
                    this->m_image_table.fetch(stencil_handle.value()),
                    0
                );
            }

            // check everything worked
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                SIREN_ASSERT(false, "Framebuffer could not be created.");
            }

            this->m_framebuffer_table.link(fb_handle, framebuffer);
        }
    );

    return Framebuffer{
        this,
        fb_handle,
        descriptor,
        std::move(color.img),
        std::move(depth.img),
        std::move(stencil.img)
    };
}

auto OpenGLDevice::destroy_framebuffer(const FramebufferHandle handle) -> void {
    const auto api_handle = m_framebuffer_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
}

auto OpenGLDevice::create_shader(const ShaderDescriptor& descriptor) -> Shader {
    SIREN_ASSERT(descriptor.source.contains(ShaderStage::Vertex), "Cannot create a Shader without a Vertex Shader");
    SIREN_ASSERT(descriptor.source.contains(ShaderStage::Fragment), "Cannot create a Shader without a Fragment Shader");

    const auto shader_handle = m_shader_table.reserve();

    render_thread().spawn(
        [descriptor, shader_handle, this] {
            // debug callbacks dont handle shader compilation
            GLint success;
            char err_info[512];

            std::vector<GLuint> shader_ids;
            shader_ids.reserve(descriptor.source.size());

            // process all stages, we can be sure we have at least vertex + fragment here
            for (const auto& [stage, stage_data] : descriptor.source) {
                const GLuint shader = glCreateShader(gl::shader_stage_to_gl(stage));
                const char* raw     = stage_data.source.c_str();

                // compile and check status of shader
                glShaderSource(shader, 1, &raw, nullptr);
                glCompileShader(shader);

                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 512, nullptr, err_info);
                    m_logger->warn(
                        "{} Shader compilation failed with error message: {}",
                        utilities::to_string(stage),
                        err_info
                    );
                }

                // optionally label the shader
                if (stage_data.label.has_value()) {
                    glObjectLabel(GL_SHADER, shader, stage_data.label.value().size(), stage_data.label.value().c_str());
                }

                shader_ids.push_back(shader);
            }

            // link all stages together into a program
            const GLuint program = glCreateProgram();
            for (const auto& shader : shader_ids) {
                glAttachShader(program, shader);
            }
            glLinkProgram(program);

            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(program, 512, nullptr, err_info);
                m_logger->warn("Shader linking failed with error message: {}", err_info);
            }

            // delete all shaders since they are linked to program
            for (const auto& shader : shader_ids) {
                glDeleteShader(shader);
            }

            // cache uniforms
            i32 uniform_count = 0;
            std::flat_map<std::string, GLint> cache;

            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
            if (uniform_count != 0) {
                i32 maxNameLength = 0;
                GLsizei length    = 0;
                GLsizei count     = 0;
                GLenum type       = GL_NONE;
                glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
                const auto uniformName = std::make_unique<char[]>(maxNameLength);

                for (i32 i = 0; i < uniform_count; i++) {
                    glGetActiveUniform(program, i, maxNameLength, &length, &count, &type, uniformName.get());
                    const i32 location = glGetUniformLocation(program, uniformName.get());
                    if (location != -1) {
                        cache[std::string(uniformName.get(), length)] = location;
                    }
                }
            }

            // optionally label the shader program
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_PROGRAM, program, descriptor.label.value().size(), descriptor.label.value().data());
            }

            this->m_shader_table.link(shader_handle, program, cache);
        }
    );

    return Shader{ this, shader_handle, descriptor };
}

auto OpenGLDevice::destroy_shader(const ShaderHandle handle) -> void {
    const auto api_handle = m_shader_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::Shader });
}

auto OpenGLDevice::create_graphics_pipeline(const GraphicsPipelineDescriptor& descriptor) -> GraphicsPipeline {
    const auto pipeline_handle = m_graphics_pipeline_table.reserve();

    render_thread().spawn(
        [pipeline_handle, descriptor, this] {
            GLuint vertex_array;
            glCreateVertexArrays(1, &vertex_array);

            // optionally label the vertex array
            if (descriptor.label.has_value()) {
                glObjectLabel(
                    GL_VERTEX_ARRAY,
                    vertex_array,
                    descriptor.label.value().size(),
                    descriptor.label.value().data()
                );
            }

            for (const auto& [index, element] : descriptor.layout.get_elements() | views::enumerate) {
                // enables some element aka the layout(location = n) shader side
                glEnableVertexArrayAttrib(vertex_array, index);

                // describe the element
                // todo: stride should be in the somewhere else now :D
                glVertexArrayAttribFormat(
                    vertex_array,
                    index,
                    element.size,
                    element.type,
                    element.normalized,
                    element.offset
                );

                // link all attributes to binding index 0 for this vao.
                // use of multiple binding indices bay be useful when data
                // is spread over multiple buffers.
                //
                // the exception is the index buffer, as the vao gets
                // a special slot for this
                glVertexArrayAttribBinding(vertex_array, index, 0);
            }

            m_graphics_pipeline_table.link(pipeline_handle, vertex_array);
        }
    );

    return GraphicsPipeline{ this, pipeline_handle, descriptor };
}

auto OpenGLDevice::destroy_graphics_pipeline(const GraphicsPipelineHandle handle) -> void {
    const auto api_handle = m_graphics_pipeline_table.fetch(handle);
    m_delete_queue.push_back({ api_handle, OpenGlResourceType::GraphicsPipeline });
}

auto OpenGLDevice::flush_delete_queue() -> void {
    for (const auto& delete_request : m_delete_queue) {
        switch (delete_request.type) {
            case OpenGlResourceType::Buffer: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteBuffers(1, &delete_request.handle);
                    }
                );
                break;
            };
            case OpenGlResourceType::Image: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteTextures(1, &delete_request.handle);
                    }
                );
                break;
            }
            case OpenGlResourceType::Sampler: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteSamplers(1, &delete_request.handle);
                    }
                );
                break;
            }
            case OpenGlResourceType::Framebuffer: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteFramebuffers(1, &delete_request.handle);
                    }
                );
                break;
            }
            case OpenGlResourceType::Shader: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteProgram(delete_request.handle);
                    }
                );
                break;
            }
            case OpenGlResourceType::GraphicsPipeline: {
                render_thread().spawn(
                    [delete_request] {
                        glDeleteVertexArrays(1, &delete_request.handle);
                    }
                );
                break;
            }
        }
    }
}

auto OpenGLDevice::record_commands() -> std::unique_ptr<CommandBuffer> {
    return std::make_unique<OpenGLCommandBuffer>();
}

auto OpenGLDevice::submit(std::unique_ptr<CommandBuffer>&& command_buffer) -> void {
    auto cmd_buf = dynamic_cast<OpenGLCommandBuffer*>(*command_buffer);
    SIREN_ASSERT(cmd_buf != nullptr, "Passed in CommandBuffer of wrong type to the OpenGLDevice.");
}

} // namespace siren::platform
