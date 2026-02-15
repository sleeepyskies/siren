#pragma once

#include "renderer/vertex_layout.hpp"
#include "shader.hpp"


namespace siren::core
{
class GraphicsPipeline;
using GraphicsPipelineHandle = RenderResourceID<GraphicsPipeline>;

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

struct GraphicsPipelineDescriptor {
    /// @brief An optional label for the @ref GraphicsPipeline. Mainly used for debugging.
    std::optional<std::string> label;
    /// @brief How the shader interprets vertex data.
    VertexLayout layout;
    /// @brief The shader to use.
    StrongHandle<ShaderAsset> shader;
    /// @brief How to draw vertex data.
    PrimitiveTopology topology = PrimitiveTopology::Triangles;
    /// @brief Surface transparency type.
    AlphaMode alpha_mode = AlphaMode::Opaque;
    /// @brief Depth function.
    DepthFunction depth_function = DepthFunction::Less;
    /// @brief Whether back face is culled.
    bool back_face_culling = true;
    /// @brief Whether to perform the depth test.
    bool depth_test = true;
    /// @brief Whether to write the depth buffer.
    bool depth_write = true;
};

/**
 * @brief The GraphicsPipeline encapsulates the vertex layout of a buffer, as well
 * as any fixed functions state.
 */
class GraphicsPipeline final : public RenderResource<GraphicsPipeline> {
    using Base = RenderResource<GraphicsPipeline>;

public:
    // todo: do we need a StrongHandle<Shader>?? like... we do need ownership, but doing loads of asset server lookups isn't good

    explicit GraphicsPipeline(
        Device* device,
        GraphicsPipelineHandle handle,
        const GraphicsPipelineDescriptor& descriptor
    );
    ~GraphicsPipeline();

    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

    /// @brief Returns the @ref GraphicsPipelineDescriptor used to create this GraphicsPipeline.
    [[nodiscard]] auto descriptor() const noexcept -> const GraphicsPipelineDescriptor&;

private:
    /// @brief The data used to create this GraphicsPipeline.
    GraphicsPipelineDescriptor m_descriptor;
};
} // namespace siren::core
