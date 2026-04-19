module;

#include <utility>
#include <string>
#include <vector>

export module siren.asset:mesh_asset;

import :material_asset;
import :asset_handle;

import siren.render;

namespace siren::asset {

export struct Surface : Asset {
    Surface(
        const StrongHandle<PBRMaterialAsset>& material,
        render::Buffer&& index_buffer,
        render::Buffer&& vertex_buffer,
        const u32 index_count
    ) : material(material),
        index_buffer(std::move(index_buffer)),
        vertex_buffer(std::move(vertex_buffer)),
        index_count(index_count) { }

    /** @brief An optional name. */
    // std::string name;
    /** @brief The material to use for this surface. */
    StrongHandle<PBRMaterialAsset> material;
    /** @brief The index buffer of this surface. */
    render::Buffer index_buffer;
    /** @brief The vertex buffer of this surface. */
    render::Buffer vertex_buffer;
    /** @brief The number of indices this surface has. */
    u32 index_count;
};

/** @brief A collection of @ref Surface's forming a complete Mesh. */
export struct Mesh : Asset {
    /** @brief Name of the mesh. */
    std::string name;
    /** @brief Surfaces belonging to this mesh. */
    std::vector<StrongHandle<Surface>> surfaces;
};

} // namespace siren::asset
