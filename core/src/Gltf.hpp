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
    String name;                             ///< @brief The name of the node.
    usize index;                             ///< @brief The index of the node in the main gltf asset.
    Mat4f transform;                         ///< @brief The transform of the node.
    Option<usize> parent;                    ///< @brief The parent node of the node (if present).
    Vector<StrongHandle<GltfNode>> children; ///< @brief The children nodes of the node.
    Option<StrongHandle<Mesh>> mesh;         ///< @brief The mesh of the node.
    Option<SceneCamera> camera;              ///< @brief The camera of the node.
    // Option<AssetHandle<Light>> light;
    // Option<AssetHandle<Skin>> Skin;

};

/// @brief A collection of nodes. Similar to a prefab.
struct GltfScene : Asset {
    String name;                               ///< @brief The name of the scene.
    usize index;                               ///< @brief The index of the scene in the main gltf asset.
    Vector<StrongHandle<GltfNode>> root_nodes; ///< @brief The root nodes of the scene.
};

/// @brief Represents a loaded gltf/glb file.
struct Gltf : Asset {
    Vector<StrongHandle<GltfScene>> scenes;        ///< @brief All scenes loaded from the gltf.
    Option<StrongHandle<GltfScene>> default_scene; ///< @brief The default scene, if provided.
    Vector<StrongHandle<Mesh>> meshes;             ///< @brief All meshes loaded from the gltf.
    Vector<StrongHandle<PBRMaterial>> materials;   ///< @brief All materials loaded from the gltf.
    Vector<StrongHandle<Texture>> textures;        ///< @brief All textures loaded from the gltf.
    Vector<StrongHandle<GltfNode>> nodes;          ///< @brief All nodes loaded from the gltf.
    Vector<SceneCamera> cameras;                   ///< @brief All cameras loaded from the gltf.
    // Vector<AssetHandle<Light>> lights;
    // Vector<AssetHandle<Skin>> skins;
};

} // namespace siren::core
