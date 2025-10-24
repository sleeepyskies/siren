#include "Mesh.hpp"

namespace siren::core
{

Ref<VertexArray> Mesh::getVertexArray() const
{
    return m_vertexArray;
}

void Mesh::setVertexArray(const Ref<VertexArray>& vertexArray)
{
    m_vertexArray = vertexArray;
}

AssetHandle Mesh::getMaterialHandle() const
{
    return m_materialHandle;
}

glm::mat4 Mesh::getTransform() const
{
    return m_transform;
}

void Mesh::setTransform(const glm::mat4& transform)
{
    m_transform = transform;
}

void Mesh::setMaterial(const AssetHandle materialHandle)
{
    m_materialHandle = materialHandle;
}

} // namespace siren::core
