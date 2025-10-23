#pragma once

#include "assets/Asset.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

class Mesh;

/**
 * @brief A collection of meshes.
 */
class Model : public assets::Asset
{
public:
    ASSET_TYPE(assets::AssetType::MODEL);
    explicit Model(const std::string& name);
    ~Model() override = default;

    /// @brief Adds the given mesh to this model.
    void addMesh(const Mesh& mesh);
    /// @brief Returns a read only reference to all of this Model's Meshes
    const std::vector<Mesh>& getMeshes() const;

private:
    /// @brief A vector of meshes
    std::vector<Mesh> m_meshes{};
};

} // namespace siren::geometry
