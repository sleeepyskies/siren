#pragma once

#include "assets/Asset.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

/**
 * @brief A collection of meshes.
 */
class Model final : public assets::Asset
{
public:
    ASSET_TYPE(assets::AssetType::MODEL);
    explicit Model(const std::string& name);
    ~Model() override = default;

    /// @brief Adds a mesh to this model. Returns true if the mesh could be added, false otherwise
    bool addMesh(const assets::AssetHandle& meshHandle);
    /// @brief Returns all mesh asset handles belonging to this Model.
    std::vector<assets::AssetHandle> getMeshes() const;

private:
    /// @brief A vector of handles of the meshes belonging to this Model
    std::vector<assets::AssetHandle> m_meshHandles{};
};

} // namespace siren::geometry
