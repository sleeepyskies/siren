#include "GraphicsPipeline.hpp"

#include "../shaders/ShaderUtils.hpp"


namespace siren::core
{
GraphicsPipeline::GraphicsPipeline(const Properties& properties, const std::string& name) : Asset(name),
    m_properties(properties), m_vertexArrayID(0) {
    glCreateVertexArrays(1, &m_vertexArrayID);

    u32 index = 0;
    for (const auto& element : m_properties.layout.get_elements()) {
        // enables some element aka the layout(location = n) shader side
        glEnableVertexArrayAttrib(m_vertexArrayID, index);

        // describe the element
        // todo: stride should be in the somewhere else now :D
        glVertexArrayAttribFormat(
            m_vertexArrayID,
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
        glVertexArrayAttribBinding(m_vertexArrayID, index++, 0);
    }
}

GraphicsPipeline::~GraphicsPipeline() {
    glDeleteVertexArrays(1, &m_vertexArrayID);
}

PrimitiveTopology GraphicsPipeline::get_topology() const {
    return m_properties.topology;
}

void GraphicsPipeline::bind() const {
    if (!m_properties.shader) {
        wrn("Cannot bind pipeline, shader asset handle is not valid.");
        return;
    }

    m_properties.shader->Bind();
    glBindVertexArray(m_vertexArrayID);

    switch (m_properties.alphaMode) {
        case AlphaMode::Opaque: {
            glEnable(GL_BLEND);
            break;
        }
        case AlphaMode::Blend: {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
    }

    glDepthFunc(depthFunctionToEnum(m_properties.depthFunction));

    if (m_properties.backFaceCulling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (m_properties.depthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    if (m_properties.depthWrite) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

VertexLayout GraphicsPipeline::get_layout() const {
    return m_properties.layout;
}

Ref<Shader> GraphicsPipeline::get_shader() const {
    return m_properties.shader;
}

u32 GraphicsPipeline::get_stride() const {
    return m_properties.layout.get_vertex_stride();
}

u32 GraphicsPipeline::get_vertex_array_id() const {
    return m_vertexArrayID;
}
} // namespace siren::core
