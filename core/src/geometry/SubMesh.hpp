#pragma once

#include "geometry/Material.hpp"
#include "renderer/Buffer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class SubMesh
{
public:
    SubMesh(const Ref<Material>& material, const Ref<renderer::VertexArray>& VAO)
        : m_material(material), m_vertexArray(VAO)
    {
    }

    Ref<renderer::VertexArray> getVertexArray() const
    {
        return m_vertexArray;
    }

    Ref<Material> getMaterial() const
    {
        return m_material;
    }

private:
    Ref<Material> m_material                 = nullptr;
    Ref<renderer::VertexArray> m_vertexArray = makeRef<renderer::VertexArray>();
};

} // namespace core::geometry
