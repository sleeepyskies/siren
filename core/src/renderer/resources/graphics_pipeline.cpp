#include "graphics_pipeline.hpp"

#include "renderer/device.hpp"


namespace siren::core
{
GraphicsPipeline::GraphicsPipeline(
    Device* device,
    const GraphicsPipelineHandle handle,
    const GraphicsPipelineDescriptor& descriptor
) : Base(device, handle), m_descriptor(descriptor) { }

GraphicsPipeline::~GraphicsPipeline() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_graphics_pipeline(m_handle);
    }
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : Base(std::move(other)),
      m_descriptor(std::move(other.m_descriptor)) { }

GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept {
    if (this != &other) {
        // cleanup old buffer
        if (m_device && m_handle.is_valid()) {
            m_device->destroy_graphics_pipeline(m_handle);
        }

        Base::operator=(std::move(other));
        m_descriptor = std::move(other.m_descriptor);
    }
    return *this;
}

auto GraphicsPipeline::descriptor() const noexcept -> const GraphicsPipelineDescriptor& { return m_descriptor; }

auto GraphicsPipeline::bind() const -> void {
    if (!m_description.shader.is_valid()) {
        Logger::renderer->warn("Cannot bind pipeline, shader asset handle is not valid.");
        return;
    }

    const auto shader = Locator<AssetServer>::locate().get(m_description.shader);
    if (!shader) {
        Logger::renderer->error("Could not retrieve shader from AssetServer");
    }

    shader->shader.bind();
    glBindVertexArray(m_vertex_array_id);

    switch (m_description.alpha_mode) {
        case AlphaMode::Opaque: {
            glEnable(GL_BLEND);
            break;
        }
        case AlphaMode::Blend: {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        case AlphaMode::Mask: {
            // shader has to handle discarding of fragments
            glEnable(GL_BLEND);
            break;
        }
    }

    glDepthFunc(depthFunctionToEnum(m_description.depth_function));

    if (m_description.back_face_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (m_description.depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    if (m_description.depth_write) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

} // namespace siren::core
