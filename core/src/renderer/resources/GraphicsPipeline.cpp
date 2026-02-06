#include "GraphicsPipeline.hpp"

#include "renderer/shaders/ShaderUtils.hpp"

#include "assets/AssetServer.hpp"


namespace siren::core
{
GraphicsPipeline::GraphicsPipeline(const Description& description) : m_description(description), m_vertex_array_id(0) {
    glCreateVertexArrays(1, &m_vertex_array_id);

    u32 index = 0;
    for (const auto& element : m_description.layout.get_elements()) {
        // enables some element aka the layout(location = n) shader side
        glEnableVertexArrayAttrib(m_vertex_array_id, index);

        // describe the element
        // todo: stride should be in the somewhere else now :D
        glVertexArrayAttribFormat(
            m_vertex_array_id,
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
        glVertexArrayAttribBinding(m_vertex_array_id, index++, 0);
    }
}

GraphicsPipeline::~GraphicsPipeline() {
    glDeleteVertexArrays(1, &m_vertex_array_id);
}

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

auto GraphicsPipeline::layout() const -> VertexLayout { return m_description.layout; }
auto GraphicsPipeline::shader() const -> StrongHandle<ShaderAsset> { return m_description.shader; }
auto GraphicsPipeline::topology() const -> PrimitiveTopology { return m_description.topology; }
auto GraphicsPipeline::vertex_array_id() const -> u32 { return m_vertex_array_id; }
} // namespace siren::core
