module;

#include <utility>
#include <string>
#include <vector>

export module siren.render:mesh;

import :buffer;
import :material;

import siren.asset.asset;
import siren.asset.handle;

namespace siren::render {

export struct Surface : asset::Asset {
    Surface(
        const asset::StrongHandle<PBRMaterial>& material,
        Buffer&& index_buffer,
        Buffer&& vertex_buffer,
        const u32 index_count
    ) : material(material),
        index_buffer(std::move(index_buffer)),
        vertex_buffer(std::move(vertex_buffer)),
        index_count(index_count) { }

    /** @brief An optional name. */
    // std::string name;
    /** @brief The material to use for this surface. */
    asset::StrongHandle<PBRMaterial> material;
    /** @brief The index buffer of this surface. */
    Buffer index_buffer;
    /** @brief The vertex buffer of this surface. */
    Buffer vertex_buffer;
    /** @brief The number of indices this surface has. */
    u32 index_count;
};

/** @brief A collection of @ref Surface's forming a complete Mesh. */
export struct Mesh : asset::Asset {
    /** @brief Name of the mesh. */
    std::string name;
    /** @brief Surfaces belonging to this mesh. */
    std::vector<asset::StrongHandle<Surface>> surfaces;
};

} // namespace siren::render
