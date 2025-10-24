/**
 * @file Mesh.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "renderer/buffer/VertexArray.hpp"

namespace siren::core
{

/**
 * @brief A basic mesh. Holds a Transform, a @ref VertexArray and a Material.
 */
class Mesh
{
public:
    Mesh()          = default;
    virtual ~Mesh() = default;

    /// @brief Returns this meshes transform relative to its root.
    glm::mat4 getTransform() const;
    /// @brief Sets this meshes transform.
    void setTransform(const glm::mat4& transform);
    /// @brief Returns this meshes material it should be rendered with.
    AssetHandle getMaterialHandle() const;
    /// @brief Sets this meshes material.
    void setMaterial(AssetHandle materialHandle);
    /// @brief Returns this meshes underlying geometric data.
    Ref<VertexArray> getVertexArray() const;
    /// @brief Sets this meshes vertex array.
    void setVertexArray(const Ref<VertexArray>& vertexArray);

private:
    glm::mat4 m_transform{1};
    AssetHandle m_materialHandle   = utilities::UUID::invalid();
    Ref<VertexArray> m_vertexArray = nullptr;
};

} // namespace siren::core
