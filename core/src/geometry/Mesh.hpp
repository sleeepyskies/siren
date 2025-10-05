#pragma once

#include "geometry/Material.hpp"
#include "renderer/Buffer.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

class Mesh final
{
public:
    Mesh(const Ref<Material>& material, const Ref<renderer::VertexArray>& VAO,
         const glm::mat4& localTransform)
        : m_localTransform(localTransform), m_material(material), m_vertexArray(VAO)
    {
    }

    Ref<renderer::VertexArray> getVertexArray() const;
    Ref<Material> getMaterial() const;
    glm::mat4 getLocalTransform() const;

private:
    glm::mat4 m_localTransform{ 1 };
    Ref<Material> m_material                 = nullptr;
    Ref<renderer::VertexArray> m_vertexArray = nullptr;
};

} // namespace siren::geometry
