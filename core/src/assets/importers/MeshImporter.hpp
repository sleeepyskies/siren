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
    static MeshImporter Create(const Path& path, ImportContext context);

    /// @brief Apply Siren default configuration settings.
    MeshImporter& Defaults();
    /// @brief Triangulates the mesh data.
    MeshImporter& Triangulate();
    /// @brief Generates normals if not present.
    MeshImporter& GenerateNormals();
    /// @brief Calculates tangents and bitangents if not present.
    MeshImporter& CalculateTangentSpace();
    /// @brief Combine meshes for fewer draw calls and reorders triangles for better efficiency.
    MeshImporter& OptimizeMeshes();
    /// @brief Removes zero-area or invalid triangles and joins identical vertices.
    MeshImporter& CleanMeshes();

    /// @brief Loads and returns the mesh. Returns nullptr on fail.
    Ref<Mesh> Load();

private:
    MeshImporter(const Path& path, ImportContext context);
    Path m_path;
    ImportContext m_context;
    u32 m_postProcessFlags = 0;

    const aiScene* m_scene         = nullptr;
    const AssetHandle m_meshHandle = AssetHandle::create();
    Ref<Mesh> m_mesh               = nullptr;
    Vector<AssetHandle> m_materials{ };

    void loadMaterials();
    void loadMeshes() const;

    bool m_success = true;
};
} // namespace siren::core::importer
