#include "Mesh.hpp"

namespace siren::geometry
{

Ref<renderer::VertexArray> Mesh::getVertexArray() const
{
    return m_vertexArray;
}

Ref<Material> Mesh::getMaterial() const
{
    return m_material;
}

glm::mat4 Mesh::getLocalTransform() const
{
    return m_localTransform;
}

} // namespace siren::geometry