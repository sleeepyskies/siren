#include "GraphicsPipeline.hpp"

#include "assets/AssetModule.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
GraphicsPipeline::GraphicsPipeline(const Properties& properties, const std::string& name) : Asset(name),
    m_properties(properties), m_vertexArrayID(0)
{
    glCreateVertexArrays(1, &m_vertexArrayID);

    u32 index = 0;
    for (const auto& element : m_properties.layout.GetElements()) {
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

GraphicsPipeline::~GraphicsPipeline()
{
    glDeleteVertexArrays(1, &m_vertexArrayID);
}

PrimitiveTopology GraphicsPipeline::GetTopology() const
{
    return m_properties.topology;
}

void GraphicsPipeline::Bind() const
{
    if (!m_properties.shader) {
        wrn("Cannot bind pipeline, shader asset handle is not valid.");
        return;
    }

    m_properties.shader->bind();
    glBindVertexArray(m_vertexArrayID);

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
}

VertexLayout GraphicsPipeline::GetLayout() const
{
    return m_properties.layout;
}

Ref<Shader> GraphicsPipeline::GetShader() const
{
    return m_properties.shader;
}

u32 GraphicsPipeline::GetStride() const
{
    return m_properties.layout.GetVertexStride();
}

u32 GraphicsPipeline::GetVertexArrayID() const
{
    return m_vertexArrayID;
}
} // namespace siren::core
