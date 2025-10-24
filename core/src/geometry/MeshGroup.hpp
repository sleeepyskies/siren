/**
 * @brief MeshGroup.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "geometry/Mesh.hpp"

namespace siren::core
{

/**
 * @brief A MeshGroup is a collection of @ref Mesh's
 */
class MeshGroup : public Asset
{
public:
    ASSET_TYPE(AssetType::MESH);
    explicit MeshGroup(const std::string& name, const std::vector<Mesh>& meshes)
        : Asset(name), m_meshes(meshes){};

    /// @brief Returns a read only reference of this group's meshes.
    const std::vector<Mesh>& getMeshes() const;
    /// @brief Returns this group's transform.
    glm::mat4 getTransform() const;

private:
    std::vector<Mesh> m_meshes;
};

} // namespace siren::core
