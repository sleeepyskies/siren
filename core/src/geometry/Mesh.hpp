#pragma once

#include "geometry/Material.hpp"
#include "renderer/Buffer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Mesh final
{
public:
    Mesh(const assets::AssetHandle& materialHandle, const Ref<renderer::VertexArray>& VAO)
        : m_materialHandle(materialHandle), m_vertexArray(VAO)
    {
    }

    Ref<renderer::VertexArray> getVertexArray() const
    {
        return m_vertexArray;
    }

    assets::AssetHandle getMaterialHandle() const
    {
        return m_materialHandle;
    }

private:
    assets::AssetHandle m_materialHandle;
    Ref<renderer::VertexArray> m_vertexArray = makeRef<renderer::VertexArray>();
};

} // namespace core::geometry
