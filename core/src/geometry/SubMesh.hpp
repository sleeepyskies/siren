#pragma once

#include "geometry/Material.hpp"
#include "renderer/Buffer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class SubMesh final : public assets::Asset
{
public:
    SubMesh(const std::string& name, const assets::AssetHandle& materialHandle,
            const Ref<renderer::VertexArray>& VAO)
        : Asset(name), m_materialHandle(materialHandle), m_vertexArray(VAO)
    {
    }

    ASSET_TYPE(assets::AssetType::Mesh);

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
