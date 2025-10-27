/**
 * @file MeshImporter.hpp
 */
#pragma once

#include "ImportContext.hpp"
#include "geometry/Mesh.hpp"
#include "utilities/spch.hpp"

class aiScene;

namespace siren::core
{

/**
 * @brief Used to import Meshes.
 * A new MeshImporter instance should be instantiated for each import.
 */
class MeshImporter
{
public:
    /// @brief Creates a new MeshImporter instance.
    static MeshImporter create(const Path& path, ImportContext context);

    /// @brief Apply Siren default configuration settings.
    MeshImporter& defaults();
    /// @brief Triangulates the mesh data.
    MeshImporter& triangulate();
    /// @brief Generates normals if not present.
    MeshImporter& generateNormals();
    /// @brief Calculates tangents and bitangents if not present.
    MeshImporter& calculateTangentSpace();
    /// @brief Combine meshes for fewer draw calls and reorders triangles for better efficiency.
    MeshImporter& optimizeMeshes();
    /// @brief Removes zero-area or invalid triangles and joins identical vertices.
    MeshImporter& cleanMeshes();

    /// @brief Loads and returns the mesh. Returns nullptr on fail.
    Ref<Mesh> load();

private:
    MeshImporter(const Path& path, ImportContext context);
    Path m_path;
    ImportContext m_context;
    u32 m_postProcessFlags = 0;

    const aiScene* m_scene = nullptr;
    AssetHandle m_meshHandle{};
    Ref<Mesh> m_mesh = nullptr;
    std::vector<AssetHandle> m_materials{};

    void loadMaterials();
    void loadMeshes() const;

    bool m_success = true;
};
} // namespace siren::core::importer
