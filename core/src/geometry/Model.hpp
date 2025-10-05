#pragma once

#include "geometry/Mesh.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
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

    // TODO: remove this, just for debugging
    void translate(glm::vec3 dir, float amt);
    void scale(float scale);

private:
    glm::mat4 m_globalTransform{ 1 };
    std::vector<Mesh> m_meshes{};
};

} // namespace siren::geometry
