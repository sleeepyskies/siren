#pragma once

#include "geometry/SubMesh.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Mesh final : public assets::Asset
{
public:
    explicit Mesh(const std::string& name) : Asset(name)
    {
    }
    ~Mesh() override = default;

    ASSET_TYPE(assets::AssetType::Model);

    void addSubmesh(const SubMesh& mesh)
    {
        m_submeshes.push_back(mesh);
    }

    std::vector<SubMesh> getSubMeshes() const
    {
        return m_submeshes;
    }

    glm::mat4 getGlobalTransform() const
    {
        return m_globalTransform;
    }

private:
    glm::mat4 m_globalTransform{ 1 };
    std::vector<SubMesh> m_submeshes{};
};

} // namespace core::geometry
