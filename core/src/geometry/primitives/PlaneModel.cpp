#include "PlaneModel.hpp"

#include "../../../../editor/src/EditorApp.hpp"
#include "core/Application.hpp"
#include "renderer/buffer/IndexBuffer.hpp"
#include "renderer/buffer/VertexArray.hpp"
#include "renderer/buffer/VertexBuffer.hpp"

#include <geometry/Mesh.hpp>

const static std::vector<float> v1 = { -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
const static std::vector<float> v2 = { 0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
const static std::vector<float> v3 = { 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
const static std::vector<float> v4 = { -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
const static std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

namespace siren::geometry
{

PlaneModel::PlaneModel(const PlaneModelParams& params)
    : PrimitiveModel(PrimitiveType::PLANE), m_params(params)
{
    regenerate();
}

PlaneModel::PlaneModel() : PrimitiveModel(PrimitiveType::PLANE), m_params(PlaneModelParams())
{
    regenerate();
}

PlaneModel::PlaneModelParams PlaneModel::getParams() const
{
    return m_params;
}

void PlaneModel::setParams(const PlaneModelParams params)
{
    m_params = params;
    regenerate();
}

void PlaneModel::regenerate()
{
    std::vector<Byte> vertexData{};

    renderer::VertexBufferLayout bufferLayout{};
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::POSITION });
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::NORMAL });
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::TANGENT });
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::BITANGENT });
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::TEXTUREUV });
    bufferLayout.addVertexAttribute({ renderer::ShaderAttribute::COLOR });

    auto p1 = v1;
    p1[0] *= m_params.width;
    p1[2] *= m_params.depth;
    auto p2 = v2;
    p2[0] *= m_params.width;
    p2[2] *= m_params.depth;
    auto p3 = v3;
    p3[0] *= m_params.width;
    p3[2] *= m_params.depth;
    auto p4 = v4;
    p4[0] *= m_params.width;
    p4[2] *= m_params.depth;

    vertexData.insert(vertexData.end(),
                      reinterpret_cast<const Byte*>(&p1),
                      reinterpret_cast<const Byte*>(&p1) + p1.size());
    vertexData.insert(vertexData.end(),
                      reinterpret_cast<const Byte*>(&p2),
                      reinterpret_cast<const Byte*>(&p2) + p2.size());
    vertexData.insert(vertexData.end(),
                      reinterpret_cast<const Byte*>(&p3),
                      reinterpret_cast<const Byte*>(&p3) + p3.size());
    vertexData.insert(vertexData.end(),
                      reinterpret_cast<const Byte*>(&p4),
                      reinterpret_cast<const Byte*>(&p4) + p4.size());

    const auto vertexBuffer = makeRef<renderer::VertexBuffer>(vertexData);
    const auto indexBuffer  = makeRef<renderer::IndexBuffer>(indices);
    const auto vertexArray  = makeRef<renderer::VertexArray>();

    vertexArray->linkIndexBuffer(indexBuffer);
    vertexArray->linkVertexBuffer(vertexBuffer);

    Ref<renderer::Material> material = makeRef<renderer::Material>("plane");
    // this is a terrible solution, PlaneModel should not register its own material
    editor::EditorApp::get().getAssetManager().getAssetRegistry().registerAsset();
    const Mesh planeMesh{ materialHandle, vertexArray, glm::mat4{ 1 } };
    addMesh(planeMesh);

    // todo: for now, we ignore subdivisions
}

} // namespace siren::geometry