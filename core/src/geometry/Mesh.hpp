#pragma once

#include "assets/Asset.hpp"
#include "geometry/Material.hpp"
#include "renderer/buffer/VertexArray.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

/**
 * @brief A collection of geometry data belonging to a Model. Also provides a transform relative to
 * its parent Model, as well as Material parameters for rendering.
 */
class Mesh final : public assets::Asset
{
public:
    ASSET_TYPE(assets::AssetType::MESH);
    Mesh(const std::string& name, const Ref<Material>& material,
         const Ref<renderer::VertexArray>& VAO, const glm::mat4& localTransform);
    ~Mesh() override = default;

    /// @brief Returns this meshes transform relative to its root
    glm::mat4 getLocalTransform() const;
    /// @brief Returns this meshes material it should be rendered with
    Ref<Material> getMaterial() const;
    /// @brief Returns this meshes underlying geometric data
    Ref<renderer::VertexArray> getVertexArray() const;

private:
    /// @brief This meshes transform relative to its root
    glm::mat4 m_localTransform{ 1 };
    /// @brief The material parameters this mesh should be rendered with
    Ref<Material> m_material                 = nullptr;
    /// @brief This meshes underlying geometry data
    Ref<renderer::VertexArray> m_vertexArray = nullptr;
};

} // namespace siren::geometry
