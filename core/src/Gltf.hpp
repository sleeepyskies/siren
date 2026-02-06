#pragma once

#include "assets/Asset.hpp"
#include "core/Core.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/PBRMaterial.hpp"


namespace siren::core
{

/// @brief A camera. Can have either orthographic or perspective projection.
struct SceneCamera {
    f32 z_far;  ///< @brief The far plane distance. Set to 0 if infinite.
    f32 z_near; ///< @brief The near plane distance.

    union {
        struct {
            f32 aspect_ratio; ///< @brief The aspect ratio. Set to 0 if not present.
            f32 y_fov;        ///< @brief The vertical fov.
        } perspective;

        struct {
            f32 x_mag; ///< @brief The horizontal magnification of the camera.
            f32 y_mag; ///< @brief The horizontal magnification of the camera.
        } orthographic;
    };

    enum Type { Perspective, Orthographic } type; ///< @brief The type of the camera.
};

/// @brief A single node in the scene hierarchy.
struct GltfNode : Asset {
    std::string name;                             ///< @brief The name of the node.
    usize index;                                  ///< @brief The index of the node in the main gltf asset.
    glm::mat4 transform;                          ///< @brief The transform of the node.
    std::optional<WeakHandle> parent;             ///< @brief The parent node of the node (if present).
    std::vector<StrongHandle<GltfNode>> children; ///< @brief The children nodes of the node.
    std::optional<StrongHandle<Mesh>> mesh;       ///< @brief The mesh of the node.
    std::optional<SceneCamera> camera;            ///< @brief The camera of the node.
    // std::optional<AssetHandle<Light>> light;
    // std::optional<AssetHandle<Skin>> Skin;

};

/// @brief A collection of nodes. Similar to a prefab.
struct GltfScene : Asset {
    GltfScene(
        const std::string& name,
        const usize index,
        std::vector<StrongHandle<GltfNode>>&& root_nodes
    ) : name(name),
        index(index),
        root_nodes(std::move(root_nodes)) { }

    std::string name;                               ///< @brief The name of the scene.
    usize index;                                    ///< @brief The index of the scene in the main gltf asset.
    std::vector<StrongHandle<GltfNode>> root_nodes; ///< @brief The root nodes of the scene.
};

/// @brief Represents a loaded gltf/glb file.
struct Gltf : Asset {
    Gltf(
        std::vector<StrongHandle<GltfScene>>&& scenes,
        std::optional<StrongHandle<GltfScene>>&& default_scene,
        std::vector<StrongHandle<Mesh>>&& meshes,
        std::vector<StrongHandle<PBRMaterial>>&& materials,
        std::vector<StrongHandle<GltfNode>>&& nodes,
        std::vector<SceneCamera>&& cameras
    ) : scenes(std::move(scenes)),
        default_scene(std::move(default_scene)),
        meshes(std::move(meshes)),
        materials(std::move(materials)),
        nodes(std::move(nodes)),
        cameras(std::move(cameras)) { }

    std::vector<StrongHandle<GltfScene>> scenes;          ///< @brief All scenes loaded from the gltf.
    std::optional<StrongHandle<GltfScene>> default_scene; ///< @brief The default scene, if provided.
    std::vector<StrongHandle<Mesh>> meshes;               ///< @brief All meshes loaded from the gltf.
    std::vector<StrongHandle<PBRMaterial>> materials;     ///< @brief All materials loaded from the gltf.
    std::vector<StrongHandle<Texture>> textures;          ///< @brief All textures loaded from the gltf.
    std::vector<StrongHandle<GltfNode>> nodes;            ///< @brief All nodes loaded from the gltf.
    std::vector<SceneCamera> cameras;                     ///< @brief All cameras loaded from the gltf.
    // std::vector<AssetHandle<Light>> lights;
    // std::vector<AssetHandle<Skin>> skins;
};

} // namespace siren::core
