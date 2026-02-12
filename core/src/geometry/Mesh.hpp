/**
 * @file Mesh.hpp
 */
#pragma once

#include "assets/asset.hpp"
#include "renderer/PBRMaterial.hpp"
#include "renderer/resources/Buffer.hpp"


namespace siren::core
{
struct Surface : Asset {
    Surface(
        const StrongHandle<PBRMaterial>& material,
        Buffer&& index_buffer,
        Buffer&& vertex_buffer,
        const u32 index_count
    ) : material(material), index_buffer(std::move(index_buffer)), vertex_buffer(std::move(vertex_buffer)),
        index_count(index_count) { }

    StrongHandle<PBRMaterial> material; ///< @brief The material to use for this surface.
    Buffer index_buffer;                ///< @brief The index buffer of this surface.
    Buffer vertex_buffer;               ///< @brief The vertex buffer of this surface.
    u32 index_count;                    ///< @brief The number of indices this surface has.
};

/// @brief A collection of surfaces forming a complete mesh.
struct Mesh : Asset {
    std::string name;                            ///< @brief Name of the mesh.
    std::vector<StrongHandle<Surface>> surfaces; ///< @brief Surfaces belonging to this mesh.
};
} // namespace siren::core
