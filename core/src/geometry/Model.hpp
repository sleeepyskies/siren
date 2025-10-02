#pragma once

#include "geometry/Mesh.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Model final : public assets::Asset
{
public:
    explicit Model(const std::string& name) : Asset(name)
    {
    }
    ~Model() override = default;

    ASSET_TYPE(assets::AssetType::MODEL);

    void addMesh(const Mesh& mesh)
    {
        m_submeshes.push_back(mesh);
    }

    std::vector<Mesh> getSubMeshes() const
    {
        return m_submeshes;
    }

    glm::mat4 getGlobalTransform() const
    {
        return m_globalTransform;
    }

private:
    glm::mat4 m_globalTransform{ 1 };
    std::vector<Mesh> m_submeshes{};
};

} // namespace core::geometry
