/**
 * @file Mesh.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "renderer/buffer/VertexArray.hpp"


namespace siren::core
{
/**
 * @brief A basic mesh. Holds a collection of surfaces.
 */
class Mesh final : public Asset
{
public:
    ASSET_TYPE(AssetType::MESH);

    explicit Mesh(const std::string& name) : Asset(name) { }

    ~Mesh() override = default;

    /**
     * @brief A collection of a transform, a @ref Material and a @ref VertexArray. Equates to a
     * single draw call.
     */
    struct Surface
    {
        glm::mat4 transform{ 1 };
        AssetHandle materialHandle   = utilities::UUID::invalid();
        Ref<VertexArray> vertexArray = nullptr;
    };

    /// @brief Adds a new surface to the mesh.
    void addSurface(const Surface& surface);

    /// @brief Adds a new surface to the mesh in place.
    template <typename... Args>
    void emplaceSurface(Args&&... args)
    {
        m_surfaces.emplace_back(std::forward<Args>(args)...);
    }

    /// @brief Returns a read only reference to this mesh's surfaces.
    const Vector<Surface>& getSurfaces() const;

    /// @brief Returns a mutable reference to this mesh's surfaces.
    Vector<Surface>& getSurfaces();

private:
    Vector<Surface> m_surfaces{ };
};
} // namespace siren::core
