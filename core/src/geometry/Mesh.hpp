/**
 * @file Mesh.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "renderer/buffer/Buffer.hpp"


namespace siren::core
{
/**
 * @brief A basic mesh. Holds a collection of surfaces.
 */
class Mesh final : public Asset
{
public:
    ASSET_TYPE(AssetType::Mesh);

    explicit Mesh(const std::string& name) : Asset(name) { }

    ~Mesh() override = default;

    /**
     * @brief A collection of a transform, a @ref Material and a @ref VertexArray. Equates to a
     * single draw call.
     */
    struct Surface
    {
        glm::mat4 transform{ 1 };
        AssetHandle materialHandle = utilities::UUID::invalid();
        Ref<Buffer> vertices       = nullptr;
        Ref<Buffer> indices        = nullptr;
        u32 indexCount;
    };

    /// @brief Adds a new surface to the mesh.
    void AddSurface(const Surface& surface);
    /// @brief Returns a read only reference to this mesh's surfaces.
    const Vector<Surface>& GetSurfaces() const;

private:
    Vector<Surface> m_surfaces{ };
};
} // namespace siren::core
