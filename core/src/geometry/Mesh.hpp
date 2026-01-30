/**
 * @file Mesh.hpp
 */
#pragma once

#include "assets/Asset.hpp"
#include "renderer/PBRMaterial.hpp"
#include "renderer/resources/Buffer.hpp"


namespace siren::core
{
struct Surface : Asset {
    StrongHandle<PBRMaterial> material; ///< @brief The material to use for this surface.
    Buffer index_buffer;               ///< @brief The index buffer of this surface.
    Buffer vertex_buffer;              ///< @brief The vertex buffer of this surface.
    u32 index_count;                   ///< @brief The number of indices this surface has.
};

/// @brief A collection of surfaces forming a complete mesh.
struct Mesh : Asset {
    String name;                           ///< @brief Name of the mesh.
    Vector<StrongHandle<Surface>> surfaces; ///< @brief Surfaces belonging to this mesh.
};
} // namespace siren::core
