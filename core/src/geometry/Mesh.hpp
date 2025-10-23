#pragma once

#include "assets/Asset.hpp"
#include "renderer/buffer/VertexArray.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

/**
 * @brief A collection of geometry data belonging to a Model. Also provides a transform relative to
 * its parent Model, as well as Material parameters for rendering. Is not considered an Asset, as
 * Meshes are always owned by a Model, which are Assets.
 */
class Mesh
{
public:
    Mesh(assets::AssetHandle materialHandle, const Ref<renderer::VertexArray>& vertexArray,
         const glm::mat4& localTransform);

    /// @brief Returns this meshes transform relative to its root
    glm::mat4 getLocalTransform() const;
    /// @brief Returns this meshes material it should be rendered with
    assets::AssetHandle getMaterialHandle() const;
    /// @brief Returns this meshes underlying geometric data
    Ref<renderer::VertexArray> getVertexArray() const;

private:
    /// @brief This meshes transform relative to its root
    glm::mat4 m_localTransform{ 1 };
    /// @brief The material parameters this mesh should be rendered with
    assets::AssetHandle m_materialHandle     = utilities::UUID::invalid();
    /// @brief This meshes underlying geometry data
    Ref<renderer::VertexArray> m_vertexArray = nullptr;
};

} // namespace siren::geometry
