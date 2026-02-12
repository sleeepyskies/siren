#pragma once

#include "renderer/vertex_layout.hpp"
#include "renderer/shaders/shader.hpp"


namespace siren::core
{
/// @brief Represents the drawing mode. Aka how points are interpreted and how lines
/// are drawn between them
enum class PrimitiveTopology {
    Points,        ///< @brief Draw vertices as points.
    Lines,         ///< @brief Every pair of vertices is treated as a line (1-2, 3-4, etc...).
    LineStrip,     ///< @brief Chain draw vertices as lines (1-2-3-4-5...)
    Triangles,     ///< @brief Every triple of vertices is treated as a triangle (1-2-3, 4-5-6, etc...)
    TriangleStrip, ///< @brief Vertices connected in a ribbon (0-1-2, 0-2-3, etc...)
    TriangleFan,   ///< @brief First vertex anchors (0-1-2, 0-2-3, 0-3-4, etc...)
};

/// @brief Defines how the renderer determines a pixels' transparency.
enum class AlphaMode {
    Opaque, ///< @brief Surface is fully solid. Depth always written to the z-buffer.
    Blend,  ///< @brief Semi-transparent. Colors from behind can show through.
    Mask,   ///< @brief Surface is either fully transparent or fully opaque based on a threshold.
};

/// @brief The function that determines if a fragment will pass the depth test.
enum class DepthFunction {
    Always,       ///< @brief Always pass.
    Never,        ///< @brief Never pass.
    Less,         ///< @brief Pass if new < old.
    Equal,        ///< @brief Pass if new == old.
    LessEqual,    ///< @brief Pass if new <= old.
    Greater,      ///< @brief Pass if new > old.
    GreaterEqual, ///< @brief Pass if new >= old.
    NotEqual,     ///< @brief Pass if new != old.
};

/**
 * @brief The GraphicsPipeline encapsulates the vertex layout of a buffer, as well
 * as any fixed functions state.
 */
class GraphicsPipeline {
public:
    // todo: do we need a StrongHandle<Shader>?? like... we do need ownership, but doing loads of asset server lookups isn't good

    /// @brief Data of the GraphicsPipeline.
    struct Description {
        VertexLayout layout;                                         ///< @brief How the shader interprets vertex data.
        StrongHandle<ShaderAsset> shader;                            ///< @brief The shader to use.
        PrimitiveTopology topology   = PrimitiveTopology::Triangles; ///< @brief How to draw vertex data.
        AlphaMode alpha_mode         = AlphaMode::Opaque;            ///< @brief Surface transparency type.
        DepthFunction depth_function = DepthFunction::Less;          ///< @brief Depth function.
        bool back_face_culling       = true;                         ///< @brief Whether back face is culled.
        bool depth_test              = true;                         ///< @brief Whether to perform the depth test.
        bool depth_write             = true;                         ///< @brief Whether to write the depth buffer.
    };

    explicit GraphicsPipeline(const Description& description);
    ~GraphicsPipeline();

    /// @brief Binds the GraphicsPipelines as well as its state.
    auto bind() const -> void;

    /// @brief Gets the vertex data layout.
    auto layout() const -> VertexLayout;
    /// @brief Gets the shader.
    auto shader() const -> StrongHandle<ShaderAsset>;
    /// @brief Gets vertex data topology.
    auto topology() const -> PrimitiveTopology;
    /// @brief Gets the ID of the underlying vertex array.
    auto vertex_array_id() const -> u32;

private:
    Description m_description; ///< @brief Pipeline data.
    u32 m_vertex_array_id;     ///< @brief Vertex array ID.
};
} // namespace siren::core
