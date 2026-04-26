module;

#include <glad/gl.h>
#include <vector>
#include <flat_map>
#include <ranges>
#include <libassert/assert.hpp>

export module siren.opengl:device;

import :mappings;

import siren.render;
import siren.common;
import siren.log;

namespace siren::render {

namespace ranges = std::ranges;
namespace views = std::views;

/**
 * @brief Encapsulates a mapped buffer pointer. This is used in streamed @ref Buffer's.
 */
struct MappedBufferPtr {
    /// @brief The mapped pointer.
    void* ptr = nullptr;
    /// @brief The size of the buffer.
    usize size = 0;
};

/**
 * @brief Information needed by the OpenGL backend for @ref Buffer's
 */
struct OpenGLBufferDetails {
    /// @brief The descriptor of the @ref Buffer.
    BufferDescriptor descriptor;
    /// @brief A mapped region of storage. Used iff the buffer type is BufferUsage::Stream.
    MappedBufferPtr buffer_ptr;
};

/**
 * @brief Information needed by the OpenGL backend for @ref Image's
 */
struct OpenGLImageDetails {
    /// @brief The descriptor of the @ref Image.
    ImageDescriptor descriptor;
};

/**
 * @brief Information needed by the OpenGL backend for @ref Sampler's
 */
struct OpenGLSamplerDetails {
    /// @brief The descriptor of the @ref Sampler.
    SamplerDescriptor descriptor;
};

/**
 * @brief Information needed by the OpenGL backend for @ref Framebuffer's
 */
struct OpenGLFramebufferDetails {
    /// @brief The descriptor of the @ref Framebuffer.
    FramebufferDescriptor descriptor;
};

/**
 * @brief Information needed by the OpenGL backend for @ref Shader's
 */
struct OpenGLShaderDetails {
    /// @brief The descriptor of the @ref Shader.
    ShaderDescriptor descriptor;
    /// @brief The uniforms of the shader cached.
    std::flat_map<std::string, GLint> uniform_cache;
};

/**
 * @brief Information needed by the OpenGL backend for @ref GraphicPipeline's
 */
struct OpenGLGraphicsPipelineDetails {
    /// @brief The descriptor of the @ref GraphicsPipeline.
    GraphicsPipelineDescriptor descriptor;
    /// @brief The cached shader program ID. Avoids performing @ref AssetServer lookups during rendering.
    GLuint shader_program_handle;
};

/**
 * @struct OpenGLRenderResourceState
 * @brief Encapsulates all @ref RenderResource state for the OpenGL backend.
 */
struct OpenGLRenderResourceState {
    /// @brief Buffer handle storage.
    RenderResourceTable<GLuint, Buffer, OpenGLBufferDetails> buffer_table;
    /// @brief Image handle storage.
    RenderResourceTable<GLuint, Image, OpenGLImageDetails> image_table;
    /// @brief Sampler handle storage.
    RenderResourceTable<GLuint, Sampler, OpenGLSamplerDetails> sampler_table;
    /// @brief Framebuffer handle storage.
    RenderResourceTable<GLuint, Framebuffer, OpenGLFramebufferDetails> framebuffer_table;
    /// @brief Shader handle storage.
    RenderResourceTable<GLuint, Shader, OpenGLShaderDetails> shader_table;
    /// @brief GraphicsPipeline handle storage.
    /// @note The GLuint stored here is not of the Pipeline, but rather the vertex array.
    ///       This is because OpenGL has no notion of a Pipeline, but we use a VA in the pipeline.
    RenderResourceTable<GLuint, GraphicsPipeline, OpenGLGraphicsPipelineDetails> graphics_pipeline_table;
};

export class OpenGLDevice final : public Device {
public:
    explicit OpenGLDevice(GLFWwindow* window);
    ~OpenGLDevice() override;

    auto wait_until_idle() const noexcept -> void override;

    auto present() const noexcept -> void override;

    [[nodiscard]] auto create_buffer(const BufferDescriptor& descriptor) -> Buffer override;
    auto destroy_buffer(BufferHandle handle) -> void override;

    [[nodiscard]] auto create_image(const ImageDescriptor& descriptor) -> Image override;
    auto destroy_image(ImageHandle handle) -> void override;

    [[nodiscard]] auto create_sampler(const SamplerDescriptor& descriptor) -> Sampler override;
    auto destroy_sampler(SamplerHandle handle) -> void override;

    [[nodiscard]] auto create_framebuffer(const FramebufferDescriptor& descriptor) -> Framebuffer override;
    auto destroy_framebuffer(FramebufferHandle handle) -> void override;

    [[nodiscard]] auto create_shader(const ShaderDescriptor& descriptor) -> Shader override;
    auto destroy_shader(ShaderHandle handle) -> void override;

    [[nodiscard]] auto create_graphics_pipeline(
        const GraphicsPipelineDescriptor& descriptor
    ) -> GraphicsPipeline override;
    auto destroy_graphics_pipeline(GraphicsPipelineHandle handle) -> void override;

    auto flush_delete_queue() -> void override;

    [[nodiscard]] auto record_resource_commands() -> ResourceCommandRecorder override;
    [[nodiscard]] auto record_render_commands() -> RenderCommandRecorder override;
    auto submit(ResourceCommandBuffer&& command_buffer) -> void override;
    auto submit(RenderCommandBuffer&& command_buffer) -> void override;

    [[nodiscard]] auto buffer_descriptor(BufferHandle handle) const -> const BufferDescriptor& override;
    [[nodiscard]] auto image_descriptor(ImageHandle handle) const -> const ImageDescriptor& override;
    [[nodiscard]] auto sampler_descriptor(SamplerHandle handle) const -> const SamplerDescriptor& override;
    [[nodiscard]] auto framebuffer_descriptor(
        FramebufferHandle handle
    ) const -> const FramebufferDescriptor& override;
    [[nodiscard]] auto shader_descriptor(ShaderHandle handle) const -> const ShaderDescriptor& override;
    [[nodiscard]] auto graphics_pipeline_descriptor(
        GraphicsPipelineHandle handle
    ) const -> const GraphicsPipelineDescriptor& override;

    /// @todo: Implement some way to query this ig
    [[nodiscard]] auto limits() const -> Limits override;

private:
    /**
     * @brief Enum listing all OpenGL GPU objects used.
     * @note We do not define a global enum for this, as different backends
     * may have a different way of doing things and not use the same GPU
     * objects.
     */
    enum class OpenGlResourceType {
        /// @brief A @ref Buffer.
        Buffer,
        /// @brief An @ref Image.
        Image,
        /// @brief A @ref Sampler.
        Sampler,
        /// @brief A @ref Framebuffer.
        Framebuffer,
        /// @brief A @ref Shader.
        Shader,
        /// @brief A @ref GraphicsPipeline.
        GraphicsPipeline,
    };

    /** @brief The main worker thread for all rendering work. */
    RenderThread m_render_thread;

    /// @brief Describes a Delete that has been requested of a GPU object.
    struct DeleteRequest {
        /// @brief The native OpenGL object handle.
        GLuint handle;
        /// @brief The resource type of the object to be deleted.
        OpenGlResourceType type;
    };

    /// @brief The state of @ref RenderResource's.
    OpenGLRenderResourceState m_state;

    /// @brief All objects queued for cleanup.
    std::vector<DeleteRequest> m_delete_queue;
};

OpenGLDevice::OpenGLDevice(GLFWwindow* window) : Device(), m_render_thread(window) { }

OpenGLDevice::~OpenGLDevice() { }

auto OpenGLDevice::wait_until_idle() const noexcept -> void {
    m_render_thread.wait_until_idle();
}

auto OpenGLDevice::present() const noexcept -> void {
    m_render_thread;
}

auto OpenGLDevice::create_buffer(const BufferDescriptor& descriptor) -> Buffer {
    ASSERT(descriptor.size > 0, "Cannot legally allocate empty buffer (sorry).");
    const auto buffer_handle = m_state.buffer_table.reserve();

    // todo: we do a copy of the whole initial buffer here, not great.

    m_render_thread.spawn(
        [buffer_handle, descriptor, this] {
            // create buffer id
            GLuint buf;
            glCreateBuffers(1, &buf);

            // optionally name it
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_BUFFER, buf, descriptor.label.value().size(), descriptor.label.value().data());
            }

            const auto flags = buffer_usage_to_flags_gl(descriptor.usage);

            // if data was passed, directly upload it
            if (descriptor.data.has_value()) {
                glNamedBufferStorage(buf, descriptor.size, descriptor.data.value().data(), flags);
            } else {
                glNamedBufferStorage(buf, descriptor.size, nullptr, flags);
            }

            // if the buffer is streamed, we also need to store a mapping pointer
            MappedBufferPtr mapped_buffer;
            mapped_buffer.size = descriptor.size;
            if (descriptor.usage == BufferUsage::Stream) {
                mapped_buffer.ptr = glMapNamedBufferRange(
                    buf,
                    0,
                    descriptor.size,
                    GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT
                );
            }

            // link proxy handle to opengl handle
            this->m_state.buffer_table.link(
                buffer_handle,
                buf,
                OpenGLBufferDetails{ .descriptor = descriptor, .buffer_ptr = mapped_buffer }
            );
        }
    );

    return Buffer{ this, buffer_handle };
}

auto OpenGLDevice::destroy_buffer(const BufferHandle handle) -> void {
    const auto api_handle = m_state.buffer_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::Buffer });
    }
    m_state.buffer_table.release(handle);
}

auto OpenGLDevice::create_image(const ImageDescriptor& descriptor) -> Image {
    ASSERT(
        descriptor.extent.width > 0 || descriptor.extent.height > 0 || descriptor.extent.depth_or_layers > 0,
        "Cannot create an empty image."
    );
    const auto image_handle = m_state.image_table.reserve();
    m_render_thread.spawn(
        [image_handle, descriptor, this] {
            const auto target = img_to_target_gl(descriptor.extent, descriptor.dimension);

            // create the image
            GLuint img;
            glCreateTextures(target, 1, &img);

            // optionally name it
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_TEXTURE, img, descriptor.label.value().size(), descriptor.label.value().data());
            }

            const auto internal_format = img_format_to_gl_internal(descriptor.format);
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
                default: ASSERT(false, "Unsupported texture target");
            }

            // assign the proxy handle to the real handle
            this->m_state.image_table.link(image_handle, img, OpenGLImageDetails{ .descriptor = descriptor });
        }
    );

    return Image{ this, image_handle };
}

auto OpenGLDevice::destroy_image(const ImageHandle handle) -> void {
    const auto api_handle = m_state.image_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::Image });
    }
    m_state.image_table.release(handle);
}

auto OpenGLDevice::create_sampler(const SamplerDescriptor& descriptor) -> Sampler {
    const auto sampler_handle = m_state.sampler_table.reserve();

    m_render_thread.spawn(
        [descriptor, sampler_handle, this]() {
            GLuint sampler;
            glCreateSamplers(1, &sampler);
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_MIN_FILTER,
                min_img_filter_to_gl(descriptor.min_filter, descriptor.mipmap_filter)
            );
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_MAG_FILTER,
                img_filter_to_gl(descriptor.min_filter)
            );

            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, img_wrap_to_gl(descriptor.s_wrap));
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, img_wrap_to_gl(descriptor.t_wrap));
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, img_wrap_to_gl(descriptor.r_wrap));

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
                img_compare_mode_to_gl(descriptor.compare_mode)
            );
            glSamplerParameteri(
                sampler,
                GL_TEXTURE_COMPARE_FUNC,
                img_compare_fn_to_gl(descriptor.compare_fn)
            );

            this->m_state.sampler_table.link(sampler_handle, sampler, OpenGLSamplerDetails{ .descriptor = descriptor });
        }
    );

    return Sampler{ this, sampler_handle };
}

auto OpenGLDevice::destroy_sampler(const SamplerHandle handle) -> void {
    const auto api_handle = m_state.sampler_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
    }
    m_state.sampler_table.release(handle);
}

/*
inline constexpr auto collect = []<std::ranges::viewable_range R> (R&& r) {
    return std::forward<R>(r) | std::ranges::to<std::vector>();
};
*/

auto OpenGLDevice::create_framebuffer(const FramebufferDescriptor& descriptor) -> Framebuffer {
    ASSERT(descriptor.width > 0, "Framebuffer must have a width of at least 1 pixel.");
    ASSERT(descriptor.height > 0, "Framebuffer must have a height of at least 1 pixel.");
    ASSERT(
        descriptor.has_depth_stencil || descriptor.num_colors > 0,
        "Framebuffer must have at least one attachment."
    );
    ASSERT(descriptor.num_colors < 32, "OpenGL Framebuffer cannot have more than 32 color attachments");

    const auto fb_handle = m_state.framebuffer_table.reserve();

    // I hate all formatters omg, wastes my time
    // @formatter:off
    auto make_color = [&] (u32 i) {
        return create_image({
                .label = make_label(descriptor.label, std::format("Color Attachment {}", i)),
                .format = ImageFormat::Color8,
                .extent = { .width = descriptor.width, .height = descriptor.height },
                .dimension = ImageDimension::D2
        });
    };

    std::vector<Image> colors =
            views::iota(0u, descriptor.num_colors)
            | views::transform(make_color)
            | ranges::to<std::vector>();

    auto depth_stencil = [&]() -> std::optional<Image> {
        if (!descriptor.has_depth_stencil) return std::nullopt;
        return this->create_image({
            .label = make_label(descriptor.label, "Depth Stencil Attachment"),
            .format = ImageFormat::DepthStencil,
            .extent = { descriptor.width, descriptor.height },
            .dimension = ImageDimension::D2
        });
    }();
    // @formatter:on

    const auto color_handles        = colors | views::transform(&Image::handle) | ranges::to<std::vector>();
    const auto depth_stencil_handle = depth_stencil.transform(&Image::handle);

    m_render_thread.spawn(
        [
            fb_handle,
            descriptor,
            color_handles = std::move(color_handles),
            depth_stencil_handle = std::move(depth_stencil_handle),
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

            // setup color attachments
            for (auto [index, handle] : color_handles | views::enumerate) {
                glNamedFramebufferTexture(
                    framebuffer,
                    GL_COLOR_ATTACHMENT0 + index,
                    this->m_state.image_table.fetch(handle),
                    0
                );
            }

            // setup depth stencil attachment
            if (descriptor.has_depth_stencil && depth_stencil_handle.has_value()) {
                glNamedFramebufferTexture(
                    framebuffer,
                    GL_DEPTH_STENCIL_ATTACHMENT,
                    this->m_state.image_table.fetch(depth_stencil_handle.value()),
                    0
                );
            }

            // check everything worked
            if (glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                ASSERT(false, "Framebuffer could not be created.");
            }

            this->m_state.framebuffer_table.link(
                fb_handle,
                framebuffer,
                OpenGLFramebufferDetails{ .descriptor = std::move(descriptor) }
            );
        }
    );

    return Framebuffer{
        this,
        fb_handle,
        std::move(colors),
        std::move(depth_stencil)
    };
}

auto OpenGLDevice::destroy_framebuffer(const FramebufferHandle handle) -> void {
    const auto api_handle = m_state.framebuffer_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::Sampler });
    }
    m_state.framebuffer_table.release(handle);
}

auto OpenGLDevice::create_shader(const ShaderDescriptor& descriptor) -> Shader {
    ASSERT(descriptor.source.contains(ShaderStage::Vertex), "Cannot create a Shader without a Vertex Shader");
    ASSERT(descriptor.source.contains(ShaderStage::Fragment), "Cannot create a Shader without a Fragment Shader");

    const auto shader_handle = m_state.shader_table.reserve();

    m_render_thread.spawn(
        [descriptor, shader_handle, this] {
            // debug callbacks dont handle shader compilation
            GLint success;
            char err_info[512];

            std::vector<GLuint> shader_ids;
            shader_ids.reserve(descriptor.source.size());

            // process all stages, we can be sure we have at least vertex + fragment here
            for (const auto& [stage, stage_data] : descriptor.source) {
                const GLuint shader = glCreateShader(shader_stage_to_gl(stage));
                const char* raw     = stage_data.source.c_str();

                // compile and check status of shader
                glShaderSource(shader, 1, &raw, nullptr);
                glCompileShader(shader);

                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 512, nullptr, err_info);
                    log::warn(
                        "{} Shader compilation failed with error message: {}",
                        stage,
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
                log::warn("Shader linking failed with error message: {}", err_info);
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
                const auto uniform_name = std::make_unique<char[]>(maxNameLength);

                for (i32 i = 0; i < uniform_count; i++) {
                    glGetActiveUniform(program, i, maxNameLength, &length, &count, &type, uniform_name.get());
                    const i32 location = glGetUniformLocation(program, uniform_name.get());
                    if (location != -1) {
                        cache[std::string(uniform_name.get(), length)] = location;
                    }
                }
            }

            // optionally label the shader program
            if (descriptor.label.has_value()) {
                glObjectLabel(GL_PROGRAM, program, descriptor.label.value().size(), descriptor.label.value().data());
            }

            this->m_state.shader_table.link(
                shader_handle,
                program,
                OpenGLShaderDetails{ .descriptor = descriptor, .uniform_cache = cache }
            );
        }
    );

    return Shader{ this, shader_handle };
}

auto OpenGLDevice::destroy_shader(const ShaderHandle handle) -> void {
    const auto api_handle = m_state.shader_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::Shader });
    }
    m_state.shader_table.release(handle);
}

auto OpenGLDevice::create_graphics_pipeline(const GraphicsPipelineDescriptor& descriptor) -> GraphicsPipeline {
    // check the shader exists
    const auto shader = Locator<AssetServer>::value().get(descriptor.shader);
    ASSERT(shader != nullptr, "Cannot create GraphicsPipeline with invalid Shader.");

    const auto pipeline_handle = m_state.graphics_pipeline_table.reserve();

    m_render_thread.spawn(
        [pipeline_handle, descriptor, program_handle = shader->shader.handle(), this] {
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

            for (const auto& [index, element] : descriptor.layout.elements() | views::enumerate) {
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

            m_state.graphics_pipeline_table.link(
                pipeline_handle,
                vertex_array,
                OpenGLGraphicsPipelineDetails{ .descriptor = descriptor, .shader_program_handle = program_handle }
            );
        }
    );

    return GraphicsPipeline{ this, pipeline_handle };
}

auto OpenGLDevice::destroy_graphics_pipeline(const GraphicsPipelineHandle handle) -> void {
    const auto api_handle = m_state.graphics_pipeline_table.fetch(handle);
    if (api_handle != 0) {
        m_delete_queue.push_back({ api_handle, OpenGlResourceType::GraphicsPipeline });
    }
    m_state.graphics_pipeline_table.release(handle);
}

auto OpenGLDevice::flush_delete_queue() -> void {
    if (m_delete_queue.empty()) { return; }

    m_render_thread.spawn(
        [delete_queue = std::move(m_delete_queue)] {
            for (const auto& delete_request : delete_queue) {
                switch (delete_request.type) {
                    case OpenGlResourceType::Buffer: {
                        glDeleteBuffers(1, &delete_request.handle);
                        break;
                    }
                    case OpenGlResourceType::Image: {
                        glDeleteTextures(1, &delete_request.handle);
                        break;
                    }
                    case OpenGlResourceType::Sampler: {
                        glDeleteSamplers(1, &delete_request.handle);
                        break;
                    }
                    case OpenGlResourceType::Framebuffer: {
                        glDeleteFramebuffers(1, &delete_request.handle);
                        break;
                    }
                    case OpenGlResourceType::Shader: {
                        glDeleteProgram(delete_request.handle);
                        break;
                    }
                    case OpenGlResourceType::GraphicsPipeline: {
                        glDeleteVertexArrays(1, &delete_request.handle);
                        break;
                    }
                }
            }
        }
    );

    m_delete_queue.clear();
}

auto OpenGLDevice::record_resource_commands() -> ResourceCommandRecorder { return ResourceCommandRecorder{ }; }

auto OpenGLDevice::record_render_commands() -> RenderCommandRecorder { return RenderCommandRecorder{ }; }

auto OpenGLDevice::submit(ResourceCommandBuffer&& command_buffer) -> void {
    m_render_thread.spawn(
        [this, cmds = std::move(command_buffer)]()mutable {
            OpenGLCommandExecutor executor{ this->m_state };
            executor.execute_resource_commands(std::move(cmds));
        }
    );
}

auto OpenGLDevice::submit(RenderCommandBuffer&& command_buffer) -> void {
    m_render_thread.spawn(
        [this, cmds = std::move(command_buffer)]() mutable {
            OpenGLCommandExecutor executor{ this->m_state };
            executor.execute_render_commands(std::move(cmds));
        }
    );
}

auto OpenGLDevice::buffer_descriptor(const BufferHandle handle) const -> const BufferDescriptor& {
    return m_state.buffer_table.extra(handle).descriptor;
}

auto OpenGLDevice::image_descriptor(const ImageHandle handle) const -> const ImageDescriptor& {
    return m_state.image_table.extra(handle).descriptor;
}

auto OpenGLDevice::sampler_descriptor(const SamplerHandle handle) const -> const SamplerDescriptor& {
    return m_state.sampler_table.extra(handle).descriptor;
}

auto OpenGLDevice::framebuffer_descriptor(const FramebufferHandle handle) const -> const FramebufferDescriptor& {
    return m_state.framebuffer_table.extra(handle).descriptor;
}

auto OpenGLDevice::shader_descriptor(const ShaderHandle handle) const -> const ShaderDescriptor& {
    return m_state.shader_table.extra(handle).descriptor;
}

auto OpenGLDevice::graphics_pipeline_descriptor(
    const GraphicsPipelineHandle handle
) const -> const GraphicsPipelineDescriptor& {
    return m_state.graphics_pipeline_table.extra(handle).descriptor;
}

auto OpenGLDevice::limits() const -> Limits { return Limits{ }; }

} // namespace siren::opengl
