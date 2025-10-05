#include "Model.hpp"

#include "core/Application.hpp"
#include "geometry/Mesh.hpp"

namespace siren::geometry
{
Model::Model(const std::string& name) : Asset(name)
{
}

bool Model::addMesh(const assets::AssetHandle& meshHandle)
{
    const auto& mesh = core::Application::get().getAssetManager().getAsset<Mesh>(meshHandle);
    if (!mesh) { return false; }
    m_meshHandles.push_back(meshHandle);
    return true;
}

std::vector<assets::AssetHandle> Model::getMeshes() const
{
    return m_meshHandles;
}

} // namespace siren::geometry
