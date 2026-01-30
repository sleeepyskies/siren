#pragma once

#include "../VertexLayout.hpp"
#include "../shaders/Shader.hpp"


namespace siren::core
{
enum class PrimitiveTopology {
    Points,
    Lines,
    Triangles,
    LineStrip,
    TriangleStrip,
    TriangleFan,
};

enum class AlphaMode {
    Opaque,
    Blend,
    Mask,
};

enum class DepthFunction {
    Always,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
};

/**
 * @brief The GraphicsPipeline encapsulates the vertex layout of a buffer, as well
 * as any fixed functions state.
 */
class GraphicsPipeline {
public:
    struct Properties {
        VertexLayout layout;
        Ref<Shader> shader;
        PrimitiveTopology topology  = PrimitiveTopology::Triangles;
        AlphaMode alphaMode         = AlphaMode::Opaque;
        DepthFunction depthFunction = DepthFunction::Less;
        bool backFaceCulling        = true;
        bool depthTest              = true;
        bool depthWrite             = true;
    };

    explicit GraphicsPipeline(const Properties& properties, const std::string& name);

    void bind() const;

    VertexLayout get_layout() const;
    Ref<Shader> get_shader() const;
    PrimitiveTopology get_topology() const;

    u32 get_stride() const;
    u32 get_vertex_array_id() const;

private:
    Properties m_properties;
    u32 m_vertexArrayID;
};
} // namespace siren::core
