#pragma once

#include "buffer/VertexLayout.hpp"
#include "shaders/Shader.hpp"


namespace siren::core
{
enum class PrimitiveTopology
{
    Points,
    Lines,
    Triangles,
    LineStrip,
    TriangleStrip,
    TriangleFan,
};

enum class AlphaMode
{
    Opaque,
    Blend,
};

enum class DepthFunction
{
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
class GraphicsPipeline final : public Asset
{
public:
    ASSET_TYPE(AssetType::GraphicsPipeline);

    struct Properties
    {
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
    ~GraphicsPipeline() override;

    void Bind() const;

    VertexLayout GetLayout() const;
    Ref<Shader> GetShader() const;
    PrimitiveTopology GetTopology() const;

    u32 GetStride() const;
    u32 GetVertexArrayID() const;

private:
    Properties m_properties;
    u32 m_vertexArrayID;
};
} // namespace siren::core
