#pragma once

#include "geometry/Mesh.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Model final : public assets::Asset
{
public:
    ASSET_TYPE(assets::AssetType::MODEL);
    explicit Model(const std::string& name, const glm::mat4& globalTransform)
        : Asset(name), m_globalTransform(globalTransform)
    {
    }
    ~Model() override = default;

    void addMesh(const Mesh& mesh);
    std::vector<Mesh> getMeshes() const;
    glm::mat4 getGlobalTransform() const;

private:
    glm::mat4 m_globalTransform{ 1 };
    std::vector<Mesh> m_meshes{};
};

} // namespace core::geometry
