#include "Model.hpp"

namespace core::geometry
{
void Model::addMesh(const Mesh& mesh)
{
    m_meshes.push_back(mesh);
}

std::vector<Mesh> Model::getMeshes() const
{
    return m_meshes;
}

glm::mat4 Model::getGlobalTransform() const
{
    return m_globalTransform;
}

} // namespace core::geometry
