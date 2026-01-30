#include <expected>

#include <glm/gtc/type_ptr.hpp>

#include "AssetLoader.hpp"

#include "Gltf.hpp"
#include "cgltf.h"
#include "utilities/stb_image.cpp"

#include "assets/AssetServer.hpp"

#include "core/FileSystem.hpp"
#include "core/Result.hpp"

#include "geometry/Mesh.hpp"
#include "geometry/VertexBufferBuilder.hpp"

#include "../../renderer/resources/GraphicsPipeline.hpp"
#include "../../renderer/resources/Texture.hpp"
#include "../../renderer/PBRMaterial.hpp"

/// For docs on GLTF see: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#indices-and-names
/// For a brief overview of GLTF see: https://www.khronos.org/files/gltf20-reference-guide.pdf

namespace siren::core
{

// ============================================================================
// == MARK: Helper functions
// ============================================================================
static auto gl_attribute_to_siren(const u32 attribute) {
    switch (attribute) {
        case cgltf_attribute_type_position: return VertexAttribute::Position;
        case cgltf_attribute_type_normal: return VertexAttribute::Normal;
        case cgltf_attribute_type_tangent: return VertexAttribute::Tangent;
        case cgltf_attribute_type_texcoord: return VertexAttribute::Texture;
        case cgltf_attribute_type_color: return VertexAttribute::Color;
        case cgltf_attribute_type_joints:
        case cgltf_attribute_type_weights:
        case cgltf_attribute_type_custom:
        case cgltf_attribute_type_invalid:
        case cgltf_attribute_type_max_enum:
        default: UNREACHABLE;
    }
}

static auto gl_filter_to_siren(const i32 filter) -> ImageSampler::Filtering {
    switch (filter) {
        case cgltf_filter_type_nearest: return ImageSampler::Filtering::Nearest;
        case cgltf_filter_type_linear: return ImageSampler::Filtering::Linear;

        // todo: dont have enum for these setup yet
        case cgltf_filter_type_nearest_mipmap_nearest:
        case cgltf_filter_type_linear_mipmap_nearest:
        case cgltf_filter_type_nearest_mipmap_linear:
        case cgltf_filter_type_linear_mipmap_linear:
        case cgltf_filter_type_undefined:
        default: UNREACHABLE;
    }
}

static auto gl_wrap_to_siren(const i32 wrap) -> ImageSampler::WrapMode {
    switch (wrap) {
        case cgltf_wrap_mode_clamp_to_edge: return ImageSampler::WrapMode::Clamp;
        case cgltf_wrap_mode_mirrored_repeat: return ImageSampler::WrapMode::Mirror;
        case cgltf_wrap_mode_repeat: return ImageSampler::WrapMode::Repeat;
        default: UNREACHABLE;
    }
}

static auto gl_alpha_mode_to_siren(const i32 alpha_mode) -> AlphaMode {
    switch (alpha_mode) {
        case cgltf_alpha_mode_opaque: return AlphaMode::Opaque;
        case cgltf_alpha_mode_mask: return AlphaMode::Mask;
        case cgltf_alpha_mode_blend: return AlphaMode::Blend;
        default: UNREACHABLE;
    }
}

static auto read_buffer_data(
    const u8* buf,
    const u32 offset,
    const u32 count,
    const u32 stride,
    const u32 component_size,
    const u32 num_components
) -> Vector<u8> {
    Vector<u8> vec;
    const u32 elem_size = component_size * num_components;
    const u32 stride_   = stride == 0 ? elem_size : stride; // if 0, data is tightly packed
    vec.resize(count * elem_size);

    u32 dest_offset = 0;
    u32 src_offset  = offset;
    for (u32 i = 0; i < count; i++) {
        std::memcpy(vec.data() + dest_offset, buf + src_offset, elem_size);
        dest_offset += elem_size;
        src_offset += stride_;
    }

    return vec;
}

// ============================================================================
// == MARK: Main Logic
// ============================================================================

static auto load_textures(
    const cgltf_data* data,
    LoadContext& ctx
) -> Result<Vector<StrongHandle<Texture>>, Error> {
    u32
    Vector<StrongHandle<Texture>> vec;
    vec.reserve(data->textures_count);

    for (i32 texture_idx = 0; texture_idx < data->textures_count; texture_idx++) {
        const auto& texture = data->textures[texture_idx];
        ImageSampler sampler{
            .minification = gl_filter_to_siren(texture.sampler->min_filter),
            .magnification = gl_filter_to_siren(texture.sampler->mag_filter),
            .s_wrap = gl_wrap_to_siren(texture.sampler->wrap_s),
            .t_wrap = gl_wrap_to_siren(texture.sampler->wrap_t),
        };

        auto handle = StrongHandle<Texture>::invalid();;
        if (texture.image->uri) {
            // load image from disk => spawn async task using ImageLoader
            const Option<Path> path = FileSystem::to_virtual(texture.image->uri, ctx.path().vfs());
            if (path.is_none()) { return Err(Error(Code::AssetNotFound)); }
            handle = ctx.load_external_asset<Texture>(AssetPath::parse(path.unwrap().string()));
        } else if (texture.image->buffer_view) {
            // load image from buffer
            if (texture.image->buffer_view->has_meshopt_compression) {
                return Err(Error(Code::AssetUnsupported));
            }
            const u8* bytes   = cgltf_buffer_view_data(texture.image->buffer_view);
            const auto size   = texture.image->buffer_view->size;
            const auto stride = texture.image->buffer_view->stride;

            i32 width, height, channels;
            const u8* img_data = stbi_load_from_memory(bytes, size, &width, &height, &channels, STBI_default);
            if (!img_data) { return Err(Error(Code::AssetCorrupted)); }

            const auto format = channels == 1
                                    ? ImageFormat::Mask8
                                    : channels == 4
                                          ? ImageFormat::LinearColor8
                                          : ImageFormat::Unknown;

            handle = ctx.add_labeled_asset(
                texture.name,
                Texture(Vector<u8>(img_data, img_data + (width * height * channels)), sampler, format, width, height)
            );

            stbi_image_free(&img_data);
        } else {
            return Err(Error(Code::AssetCorrupted));
        }

        if (!handle.is_valid()) { return Err(Error(Code::AssetNotFound)); }
        vec.emplace_back(std::move(handle));
    }

    return Ok(vec);
}

static auto load_materials(
    const cgltf_data* data,
    const Vector<StrongHandle<Texture>>& textures,
    LoadContext& ctx
) -> Vector<StrongHandle<PBRMaterial>> {
    // todo: error handle the get_texture maybe?

    const auto get_texture = [&textures, &data] (const cgltf_texture* texture) -> StrongHandle<Texture> {
        return textures[texture - data->textures];
    };

    Vector<StrongHandle<PBRMaterial>> vec;
    vec.reserve(data->materials_count);

    for (i32 material_idx = 0; material_idx < data->materials_count; material_idx++) {
        const auto& gltf_material = data->materials[material_idx];
        PBRMaterial mat;

        // metallic roughness and specular glossiness are mutually exclusive. we stick to just metallic roughness

        if (gltf_material.has_pbr_metallic_roughness) {
            const auto& pbr_mr = gltf_material.pbr_metallic_roughness;
            mat.set_base_color(create_vec4(pbr_mr.base_color_factor));
            mat.set_metallic(pbr_mr.metallic_factor);
            mat.set_roughness(pbr_mr.roughness_factor);
            if (pbr_mr.base_color_texture.texture) {
                mat.set_base_color_tex(get_texture(pbr_mr.base_color_texture.texture));
            }
            if (pbr_mr.metallic_roughness_texture.texture) {
                mat.set_metallic_roughness_tex(get_texture(pbr_mr.metallic_roughness_texture.texture));
            }
        }

        if (gltf_material.has_pbr_specular_glossiness) { /* pass */ }

        if (gltf_material.has_clearcoat) {
            const auto& cc = gltf_material.clearcoat;
            mat.set_clear_coat(cc.clearcoat_factor);
            mat.set_clear_coat_roughness(cc.clearcoat_roughness_factor);
            if (cc.clearcoat_texture.texture) {
                mat.set_clear_coat_tex(get_texture(cc.clearcoat_texture.texture));
            }
            if (cc.clearcoat_roughness_texture.texture) {
                mat.set_clear_coat_roughness_tex(get_texture(cc.clearcoat_roughness_texture.texture));
            }
            if (cc.clearcoat_normal_texture.texture) {
                mat.set_clearcoat_normal_tex(get_texture(cc.clearcoat_roughness_texture.texture));
            }
        }

        if (gltf_material.has_transmission) {
            const auto& tr = gltf_material.transmission;
            mat.set_transmission(tr.transmission_factor);
            if (tr.transmission_texture.texture) {
                mat.set_transmission_tex(get_texture(tr.transmission_texture.texture));
            }
        }

        if (gltf_material.has_volume) {
            const auto& vol = gltf_material.volume;
            mat.set_thickness(vol.thickness_factor);
            mat.set_attenuation_color(create_vec3(vol.attenuation_color));
            mat.set_attenuation_distance(vol.attenuation_distance);
            if (vol.thickness_texture.texture) {
                mat.set_thickness_texture(get_texture(vol.thickness_texture.texture));
            }
        }

        if (gltf_material.has_ior) {
            mat.set_ior(gltf_material.ior.ior);
        }

        if (gltf_material.has_specular) {
            const auto& spec = gltf_material.specular;
            mat.set_specular_factor(spec.specular_factor);
            mat.set_specular_color(create_vec3(spec.specular_color_factor));
            if (spec.specular_color_texture.texture) {
                mat.set_specular_color_tex(get_texture(spec.specular_color_texture.texture));
            }
            if (spec.specular_texture.texture) {
                mat.set_specular_tex(get_texture(spec.specular_texture.texture));
            }
        }

        if (gltf_material.has_sheen) {
            const auto& sh = gltf_material.sheen;
            mat.set_sheen_color(create_vec3(sh.sheen_color_factor));
            mat.set_sheen_roughness(sh.sheen_roughness_factor);
            if (sh.sheen_color_texture.texture) {
                mat.set_sheen_color_tex(get_texture(sh.sheen_color_texture.texture));
            }
            if (sh.sheen_roughness_texture.texture) {
                mat.set_sheen_roughness_tex(get_texture(sh.sheen_roughness_texture.texture));
            }
        }

        if (gltf_material.has_emissive_strength) {
            mat.set_emissive_strength(gltf_material.emissive_strength.emissive_strength);
        }

        if (gltf_material.has_iridescence) {
            const auto& ir = gltf_material.iridescence;
            mat.set_iridescence_factor(ir.iridescence_factor);
            mat.set_iridescence_ior(ir.iridescence_ior);
            mat.set_iridescence_min(ir.iridescence_thickness_min);
            mat.set_iridescence_max(ir.iridescence_thickness_max);
            if (ir.iridescence_texture.texture) {
                mat.set_iridescence_tex(get_texture(ir.iridescence_texture.texture));
            }
            if (ir.iridescence_thickness_texture.texture) {
                mat.set_iridescence_thickness_tex(get_texture(ir.iridescence_thickness_texture.texture));
            }
        }

        if (gltf_material.has_diffuse_transmission) {
            const auto& df = gltf_material.diffuse_transmission;
            mat.set_diffuse_transmission_factor(df.diffuse_transmission_factor);
            mat.set_diffuse_transmission_color(create_vec3(df.diffuse_transmission_color_factor));
            if (df.diffuse_transmission_texture.texture) {
                mat.set_diffuse_transmission_tex(get_texture(df.diffuse_transmission_texture.texture));
            }
            if (df.diffuse_transmission_color_texture.texture) {
                mat.set_diffuse_transmission_color_tex(get_texture(df.diffuse_transmission_color_texture.texture));
            }
        }

        if (gltf_material.has_anisotropy) {
            const auto& an = gltf_material.anisotropy;
            mat.set_anisotropy_strength(an.anisotropy_strength);
            mat.set_anisotropy_rotation(an.anisotropy_rotation);
            if (an.anisotropy_texture.texture) {
                mat.set_anisotropy_tex(get_texture(an.anisotropy_texture.texture));
            }
        }

        if (gltf_material.has_dispersion) {
            mat.set_dispersion(gltf_material.dispersion.dispersion);
        }

        if (gltf_material.normal_texture.texture) {
            mat.set_normal_tex(get_texture(gltf_material.normal_texture.texture));
        }

        if (gltf_material.occlusion_texture.texture) {
            mat.set_occlusion_tex(get_texture(gltf_material.occlusion_texture.texture));
        }

        if (gltf_material.emissive_texture.texture) {
            mat.set_emissive_tex(get_texture(gltf_material.emissive_texture.texture));
        }

        mat.set_emissive_color(create_vec3(gltf_material.emissive_factor));

        mat.set_alpha_mode(gl_alpha_mode_to_siren(gltf_material.alpha_mode));
        mat.set_alpha_cutoff(gltf_material.alpha_cutoff);
        mat.set_double_sided((bool)gltf_material.double_sided);
        mat.set_unlit((bool)gltf_material.unlit);

        vec.emplace_back(ctx.add_labeled_asset(gltf_material.name, std::move(mat)));
    }

    return vec;
}

static auto load_meshes(
    const cgltf_data* data,
    const Vector<StrongHandle<PBRMaterial>>& materials,
    LoadContext& ctx
) -> Result<Vector<StrongHandle<Mesh>>, Error> {
    Vector<StrongHandle<Mesh>> vec;
    vec.reserve(data->meshes_count);

    for (i32 mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++) {
        const auto& gltf_mesh = data->meshes[mesh_idx];
        Mesh mesh;

        for (i32 prim_idx = 0; prim_idx < gltf_mesh.primitives_count; prim_idx++) {
            const auto& gltf_prim = gltf_mesh.primitives[prim_idx];
            // only support triangles
            if (gltf_prim.type != cgltf_primitive_type_triangles) {
                return Err{ Error{ Code::AssetUnsupported } };
            }

            // dont support any compression
            if (gltf_prim.has_draco_mesh_compression) { return Err{ Error{ Code::AssetUnsupported } }; }

            const auto gltf_indices = gltf_prim.indices;
            if (!gltf_indices) { return Err{ Error{ Code::AssetUnsupported } }; }

            if (gltf_indices->component_type != cgltf_component_type_r_32u)
                return Err{ Error{ Code::AssetUnsupported, "Siren only supports u32 indices" } };
            if (gltf_indices->type != cgltf_type_scalar)
                return Err{ Error{ Code::AssetCorrupted, "Indices must be scalar types." } };
            if (!gltf_indices->buffer_view
                || !gltf_indices->buffer_view->buffer
                || !gltf_indices->buffer_view->buffer->data
            ) { return Err{ Error{ Code::AssetCorrupted, "Invalid mesh buffer data." } }; }
            if (!gltf_prim.attributes)
                return Err{ Error{ Code::AssetCorrupted, "Mesh must specify attributes." } };

            const auto idx_buffer_view = gltf_indices->buffer_view;

            auto idx_data =
                    read_buffer_data(
                        static_cast<const u8*>(idx_buffer_view->buffer->data),
                        gltf_indices->offset + idx_buffer_view->offset,
                        gltf_indices->count,
                        idx_buffer_view->stride,
                        4,
                        1
                    );

            Vector<VertexAttribute> attr_list;
            Vector<u8> positions;  // vec3
            Vector<u8> normals;    // vec3
            Vector<u8> tangents;   // vec3
            Vector<u8> bitangents; // vec3
            Vector<u8> textures;   // vec2
            Vector<u8> colors;     // vec4
            u32 element_count = 0;
            for (u32 attr_idx = 0; attr_idx < gltf_prim.attributes_count; attr_idx++) {
                const auto& gltf_attribute      = gltf_prim.attributes[attr_idx];
                const VertexAttribute attribute = gl_attribute_to_siren(gltf_attribute.type);
                attr_list.push_back(attribute);
                const auto& prim_accessor    = gltf_attribute.data;
                const auto& prim_buffer_view = prim_accessor->buffer_view;
                const auto& prim_buffer      = prim_buffer_view->data;

                const auto src            = static_cast<const u8*>(prim_buffer);
                const auto offset         = prim_accessor->offset + prim_buffer_view->offset;
                const auto count          = prim_accessor->count;
                const auto stride         = prim_buffer_view->stride;
                const auto component_size = cgltf_component_size(prim_accessor->component_type);
                const auto num_components = cgltf_num_components(prim_accessor->type);
                element_count             = glm::max(element_count, (u32)prim_accessor->count);

                switch (attribute) {
                    case VertexAttribute::Position: {
                        if (component_size != 4 || num_components != 3) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec3 float position attributes." }
                            };
                        }
                        positions = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                    case VertexAttribute::Normal: {
                        if (component_size != 4 || num_components != 3) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec3 float normal attributes." }
                            };
                        }
                        normals = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                    case VertexAttribute::Tangent: {
                        if (component_size != 4 || num_components != 3) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec3 float tangent attributes." }
                            };
                        }
                        tangents = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                    case VertexAttribute::Bitangent: {
                        if (component_size != 4 || num_components != 3) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec3 float bitangent attributes." }
                            };
                        }
                        bitangents = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                    case VertexAttribute::Texture: {
                        if (component_size != 4 || num_components != 2) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec2 float texture attributes." }
                            };
                        }
                        textures = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                    case VertexAttribute::Color: {
                        if (component_size != 4 || num_components != 4) {
                            return Err{
                                Error{ Code::AssetUnsupported, "Siren only supports vec4 float color attributes." }
                            };
                        }
                        colors = read_buffer_data(src, offset, count, stride, component_size, num_components);
                        break;
                    }
                }
            }

            const auto get_element = []<typename T> (const Vector<u8>& buf, const u32 idx) -> T {
                if (buf.empty()) { return T{ 0 }; }
                return reinterpret_cast<const T*>(buf.data())[idx];
            };

            VertexBufferBuilder vbb{ VertexLayout{ std::move(attr_list) } };
            for (u32 elem_idx = 0; elem_idx < element_count; elem_idx++) {
                vbb.push_vertex(
                    {
                        .position = get_element.operator()<glm::vec3>(positions, elem_idx),
                        .normal = get_element.operator()<glm::vec3>(normals, elem_idx),
                        .tangent = get_element.operator()<glm::vec3>(tangents, elem_idx),
                        .bitangent = get_element.operator()<glm::vec3>(bitangents, elem_idx),
                        .texture = get_element.operator()<glm::vec2>(textures, elem_idx),
                        .color = get_element.operator()<glm::vec4>(colors, elem_idx),
                    }
                );
            }

            const auto& material_handle = materials[gltf_prim.material - data->materials];
            Own<Buffer> index_buffer    = create_own<Buffer>(std::span(idx_data), BufferUsage::Static);
            Own<Buffer> vertex_buffer   = vbb.build();
            mesh.surfaces.push_back(
                Surface{
                    .material = material_handle,
                    .index_buffer = std::move(index_buffer),
                    .vertex_buffer = std::move(vertex_buffer),
                    .index_count = (u32)gltf_indices->count
                }
            );
        }

        vec.emplace_back(ctx.add_labeled_asset(gltf_mesh.name, std::move(mesh)));
    }

    return Ok(vec);
}

static auto load_cameras(
    const cgltf_data* data
) -> Result<Vector<SceneCamera>, Error> {
    Vector<SceneCamera> vec;
    vec.reserve(data->cameras_count);

    for (u32 cam_idx = 0; cam_idx < data->cameras_count; cam_idx++) {
        const auto& gltf_camera = data->cameras[cam_idx];
        if (gltf_camera.type == cgltf_camera_type_perspective) {
            const auto& cam_data = gltf_camera.data.perspective;
            vec.emplace_back(
                SceneCamera{
                    .z_far = cam_data.has_zfar ? cam_data.zfar : 0,
                    .z_near = cam_data.zfar,
                    .perspective = {
                        .aspect_ratio = cam_data.has_aspect_ratio ? cam_data.aspect_ratio : 0,
                        .y_fov = cam_data.yfov,
                    },
                    .type = SceneCamera::Perspective,
                }
            );
        } else if (gltf_camera.type == cgltf_camera_type_orthographic) {
            const auto& cam_data = gltf_camera.data.orthographic;
            vec.emplace_back(
                SceneCamera{
                    .z_far = cam_data.zfar,
                    .z_near = cam_data.zfar,
                    .orthographic = {
                        .x_mag = cam_data.xmag,
                        .y_mag = cam_data.ymag,
                    },
                    .type = SceneCamera::Orthographic
                }
            );
        } else {
            return Err{ Error{ Code::AssetCorrupted, "Camera defines incorrect type." } };
        }
    }

    return Ok(vec);
}

static auto load_nodes(
    const cgltf_data* data,
    const Vector<StrongHandle<Mesh>>& meshes,
    const Vector<SceneCamera>& cameras
) -> Result<Vector<GltfNode>> {
    Vector<GltfNode> vec;
    vec.reserve(data->nodes_count);

    const auto get_transform = [] (const cgltf_node& node)-> Mat4f {
        f32 out[16];
        cgltf_node_transform_local(&node, (cgltf_float*)out);
        return glm::make_mat4(out);
    };

    const auto get_node_idx = [data] (const cgltf_node* node_ptr) -> usize { return node_ptr - data->nodes; };

    for (u32 node_idx = 0; node_idx < data->nodes_count; node_idx++) {
        const auto& gltf_node = data->nodes[node_idx];

        const Option<StrongHandle<Mesh>> mesh = gltf_node.mesh == nullptr
                                                    ? Option<StrongHandle<Mesh>>(None)
                                                    : Some(meshes[gltf_node.mesh - data->meshes]);
        const Option<SceneCamera> camera = gltf_node.camera == nullptr
                                               ? Option<SceneCamera>(None)
                                               : Some(cameras[gltf_node.camera - data->cameras]);

        const Option<usize> parent = gltf_node.parent != nullptr
                                         ? Option<usize>(None)
                                         : Some(get_node_idx(gltf_node.parent));

        Vector<usize> children;
        children.reserve(gltf_node.children_count);

        for (usize child_idx = 0; child_idx < gltf_node.children_count; child_idx++) {
            children.emplace_back(get_node_idx(gltf_node.children[child_idx]));
        }

        vec.emplace_back(
            GltfNode{
                .name = gltf_node.name,
                .index = get_node_idx(&data->nodes[node_idx]),
                .transform = get_transform(gltf_node),
                .parent = parent,
                .children = std::move(children),
                .mesh = mesh,
                .camera = camera,
            }
        );
    }

    return Ok(vec);
}

static auto load_scenes(
    const cgltf_data* data,
    const Vector<GltfNode>& nodes,
    LoadContext& ctx
) -> Result<Vector<StrongHandle<GltfScene>>, Error> {
    u32 unnamed_cnt = 0;
    Vector<StrongHandle<GltfScene>> vec;
    vec.reserve(data->scenes_count);

    for (usize scene_idx = 0; scene_idx < data->scenes_count; scene_idx++) {
        const auto& scene = data->scenes[scene_idx];
        Vector<StrongHandle<GltfNode>> root_nodes;

        ctx.add_labeled_asset();
        GltfScene{
            .name = scene.name,
            .index = scene_idx - data->scenes_count,
            .root_nodes = std::move(root_nodes),
        };
    }
}

auto GltfLoader::load(LoadContext&& ctx) const -> Result<Unit, Error> {
    const auto data = FileSystem::to_physical(ctx.path().path()).and_then(
        [] (const Path& p) -> Option<cgltf_data*> {
            cgltf_data* data = nullptr;
            constexpr cgltf_options options{ };

            if (cgltf_parse_file(&options, p.string().c_str(), &data) != cgltf_result_success) { return None; }

            if (cgltf_validate(data) != cgltf_result_success) {
                cgltf_free(data);
                return None;
            }

            if (cgltf_load_buffers(&options, data, p.string().c_str()) != cgltf_result_success) {
                cgltf_free(data);
                return None;
            }

            return Some(data);
        }
    ).value_or(nullptr);

    if (!data) { return Err(Error(Code::AssetCorrupted)); }

    const auto textures_result = load_textures(data, ctx);
    if (textures_result.is_err()) { return Err{ textures_result.unwrap_err() }; }
    const Vector<StrongHandle<Texture>> textures = textures_result.unwrap();

    const auto materials = load_materials(data, textures, ctx);

    const auto meshes_result = load_meshes(data, materials, ctx);
    if (meshes_result.is_err()) { return Err{ meshes_result.unwrap_err() }; }
    const Vector<StrongHandle<Mesh>> meshes = meshes_result.unwrap();

    const auto cameras_result = load_cameras(data);
    if (cameras_result.is_err()) { return Err{ cameras_result.unwrap_err() }; }
    const Vector<SceneCamera> cameras = cameras_result.unwrap();

    const auto nodes_result = load_nodes(data, meshes, cameras);
    if (nodes_result.is_err()) { return Err{ nodes_result.unwrap_err() }; }
    const Vector<GltfNode> nodes = nodes_result.unwrap();

    const auto scenes_result = load_scenes(data, nodes, ctx);
    if (scenes_result.is_err()) { return Err{ scenes_result.unwrap_err() }; }
    const Vector<StrongHandle<GltfScene>> scenes = scenes_result.unwrap();

    const Option<StrongHandle<GltfScene>> default_scene = data->scene == nullptr
                                                              ? Option<StrongHandle<GltfScene>>(None)
                                                              : Some(scenes[data->scene - data->scenes]);

    Gltf gltf{
        .scenes = std::move(scenes),
        .default_scene = std::move(default_scene),
        .meshes = std::move(meshes),
        .materials = std::move(materials),
        .nodes = std::move(nodes),
        .cameras = std::move(cameras)
    };

    cgltf_free(data);
    return Ok(unit);
}

} // namespace siren::core
