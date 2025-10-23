#include "Mesh.hpp"

namespace siren::geometry
{

Mesh::Mesh(const std::string& name, const assets::AssetHandle materialHandle,
           const Ref<renderer::VertexArray>& vertexArray, const glm::mat4& localTransform)
    : Asset(name), m_localTransform(localTransform), m_materialHandle(materialHandle),
      m_vertexArray(vertexArray)
{
}

Ref<renderer::VertexArray> Mesh::getVertexArray() const
{
    return m_vertexArray;
}

assets::AssetHandle Mesh::getMaterialHandle() const
{
    return m_materialHandle;
}

glm::mat4 Mesh::getLocalTransform() const
{
    return m_localTransform;
}

} // namespace siren::geometry