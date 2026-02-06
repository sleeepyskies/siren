#pragma once

#include "renderer/VertexLayout.hpp"
#include "renderer/shaders/Shader.hpp"


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

// todo: should this be an asset?

/**
 * @brief The GraphicsPipeline encapsulates the vertex layout of a buffer, as well
 * as any fixed functions state.
 */
class GraphicsPipeline {
public:
    struct Description {
        VertexLayout layout;
        std::shared_ptr<Shader> shader;
        PrimitiveTopology topology   = PrimitiveTopology::Triangles;
        AlphaMode alphaMode          = AlphaMode::Opaque;
        DepthFunction depth_function = DepthFunction::Less;
        bool back_face_culling       = true;
        bool depth_test              = true;
        bool depth_write             = true;
    };

    explicit GraphicsPipeline(const Description& description, const std::string& name);

    void bind() const;

    VertexLayout layout() const;
    std::shared_ptr<Shader> shader() const;
    PrimitiveTopology topology() const;

    u32 stride() const;
    u32 vertex_array_id() const;

private:
    Description m_description;
    u32 m_vertex_array_id;
};
} // namespace siren::core
