#include "Model.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace siren::geometry
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

void Model::translate(const glm::vec3 dir, const float amt)
{
    const glm::vec3 translation = glm::normalize(dir) * amt;
    m_globalTransform           = glm::translate(m_globalTransform, translation);
}

void Model::scale(float scale)
{
    m_globalTransform = glm::scale(m_globalTransform, glm::vec3{ scale });
}

} // namespace siren::geometry
