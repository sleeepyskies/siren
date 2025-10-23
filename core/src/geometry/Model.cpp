#include "Model.hpp"

#include "core/Application.hpp"
#include "geometry/Mesh.hpp"

namespace siren::geometry
{
Model::Model(const std::string& name) : Asset(name)
{
}

void Model::addMesh(const Mesh& mesh)
{
    m_meshes.push_back(mesh);
}

std::vector<assets::AssetHandle> Model::getMeshHandles() const
{
    return m_meshHandles;
}

} // namespace siren::geometry
