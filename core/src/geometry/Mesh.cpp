#include "Mesh.hpp"

namespace siren::geometry
{

Mesh::Mesh(const std::string& name, const Ref<Material>& material,
           const Ref<renderer::VertexArray>& VAO, const glm::mat4& localTransform)
    : Asset(name), m_localTransform(localTransform), m_material(material), m_vertexArray(VAO)
{
}

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