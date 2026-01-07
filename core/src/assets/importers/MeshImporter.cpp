#include "MeshImporter.hpp"

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "TextureImporter.hpp"
#include "assets/Asset.hpp"
#include "assets/AssetModule.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/material/Material.hpp"
#include "utilities/spch.hpp"

#include <ranges>

#include "geometry/VertexBufferBuilder.hpp"

#include "renderer/RenderModule.hpp"


namespace siren::core
{
// ============================================================================
// == MARK: Utilities
// ============================================================================

static u32 s_importCount = 0;

static glm::mat4 aiMatrixToGlm(const aiMatrix4x4& m)
{
    // clang-format off
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
    // clang-format on
}

// ============================================================================
// == MARK: Builder Functions
// ============================================================================

MeshImporter MeshImporter::Create(const Path& path, const ImportContext context)
{
    return MeshImporter{ path, context };
}

MeshImporter& MeshImporter::Defaults()
{
    return Triangulate().GenerateNormals().CalculateTangentSpace().OptimizeMeshes().CleanMeshes();
}

MeshImporter& MeshImporter::Triangulate()
{
    m_postProcessFlags |= aiProcess_Triangulate;
    return *this;
}

MeshImporter& MeshImporter::GenerateNormals()
{
    m_postProcessFlags |= aiProcess_GenNormals;
    return *this;
}

MeshImporter& MeshImporter::CalculateTangentSpace()
{
    m_postProcessFlags |= aiProcess_CalcTangentSpace;
    return *this;
}

MeshImporter& MeshImporter::OptimizeMeshes()
{
    m_postProcessFlags |= aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality;
    return *this;
}

MeshImporter& MeshImporter::CleanMeshes()
{
    m_postProcessFlags |=
            aiProcess_FindDegenerates | aiProcess_JoinIdenticalVertices | aiProcess_FindInvalidData;
    return *this;
}

MeshImporter::MeshImporter(const Path& path, const ImportContext context)
    : m_path(path), m_context(context) { }

// ============================================================================
// == MARK: Import
// ============================================================================

Ref<Mesh> MeshImporter::Load()
{
    if (!filesystem().exists(m_path)) {
        dbg("Cannot import Mesh as {} does not exist.", m_path.string());
        return nullptr;
    }

    Assimp::Importer importer;
    m_scene = importer.ReadFile(m_path.string(), m_postProcessFlags);

    if (!m_scene) {
        dbg("Failed to load model from {}", m_path.string());
        return nullptr;
    }

    if (m_scene->mNumMeshes == 0 || !m_scene->mRootNode) {
        dbg("Failed to load model from {}", m_path.string());
        return nullptr;
    }

    const std::string name = !m_scene->mName.Empty()
                                 ? std::string(m_scene->mName.C_Str())
                                 : "Mesh_" + std::to_string(s_importCount++);

    m_mesh = create_ref<Mesh>(name);

    loadMaterials();
    loadMeshes();

    if (!m_success) {
        return nullptr;
    }

    return m_mesh;
}

// fixme: add fallback assets for safety here? fallback textures?

void MeshImporter::loadMaterials()
{
    for (i32 i = 0; i < m_scene->mNumMaterials; i++) {
        const AssetHandle materialHandle = AssetHandle::create();

        const aiMaterial* aiMat = m_scene->mMaterials[i];
        const std::string name  = !aiMat->GetName().Empty()
                                     ? std::string(aiMat->GetName().C_Str())
                                     : "Material_" + std::to_string(i);
        auto material = create_ref<Material>(name);

        // ========= Basic PBR params =========

        // base color
        {
            // first try base color, then diffuse
            aiColor3D color;
            if (aiMat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) {
                material->baseColor = glm::vec4{ color.r, color.g, color.b, 1.0f };
            } else if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
                material->baseColor = glm::vec4{ color.r, color.g, color.b, 1.0f };
            }
        }
        // roughness
        {
            float roughness;
            if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
                material->roughness = roughness;
            };
        }
        // metallic
        {
            float metallic;
            if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
                material->metallic = metallic;
            };
        }
        // emissive
        {
            aiColor3D emissive;
            if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
                material->emissive = glm::vec3{ emissive.r, emissive.g, emissive.b };
            }
        }
        // ambient occlusion not provided by assimp
        // normal scale not provided by assimp

        // ========= Textures =========

        auto loadTexture = [&] (
            const aiTextureType aiTextureType,
            const Material::TextureRole sirenTextureType,
            const ImageFormat format
        ) -> void {
            aiString texturePath;
            if (aiMat->GetTexture(aiTextureType, 0, &texturePath) != AI_SUCCESS) {
                return;
            }

            Ref<Texture2D> texture = nullptr;

            if (!Path(texturePath.C_Str()).is_absolute()) {
                texturePath = aiString{ (m_path.parent_path() / Path{ texturePath.C_Str() }).string() };
            }

            texture = TextureImporter::Create(m_scene, texturePath).SetTextureFormat(format).Load2D();

            if (!texture) { return; }

            const AssetHandle textureHandle = AssetHandle::create();
            const AssetMetaData metaData{
                .type = AssetType::Texture2D,
                .sourceData = materialHandle,
            };

            if (m_context.registerAsset(textureHandle, texture, metaData)) {
                material->setTexture(sirenTextureType, textureHandle);
                return;
            }

            dbg("Invalid Texture parsed. Cannot create mesh.");
            m_success = false;
        };

        // base color
        loadTexture(aiTextureType_BASE_COLOR, Material::TextureRole::BaseColor, ImageFormat::LinearColor8);
        // metallic roughness
        loadTexture(
            aiTextureType_GLTF_METALLIC_ROUGHNESS,
            Material::TextureRole::MetallicRoughness,
            ImageFormat::LinearColor8
        );
        if (!material->hasTexture(Material::TextureRole::Occlusion)) {
            // todo: combine textures into one METALLIC_ROUGHNESS
            aiString texturePath;
            const bool hasMetallic =
                    aiMat->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS;
            const bool hasRoughness =
                    aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texturePath) ==
                    AI_SUCCESS;
            if (hasMetallic || hasRoughness) {
                wrn(
                    "Importing asset which has separate metallic roughness textures. This is "
                    "currently not supported. Ignoring these textures."
                );
            }
        }
        // normal
        loadTexture(aiTextureType_NORMALS, Material::TextureRole::Normal, ImageFormat::LinearColor8);
        // emission
        loadTexture(aiTextureType_EMISSION_COLOR, Material::TextureRole::Emission, ImageFormat::LinearColor8);
        // occlusion
        loadTexture(aiTextureType_AMBIENT_OCCLUSION, Material::TextureRole::Occlusion, ImageFormat::LinearColor8);

        AssetMetaData metaData{
            .type = AssetType::Material,
            .sourceData = m_meshHandle,
        };

        if (m_context.registerAsset(materialHandle, material, metaData)) {
            m_materials.push_back(materialHandle);
        } else {
            dbg("Cannot assign material a null shader.");
            m_success = false;
            return;
        }
    }
}

void MeshImporter::loadMeshes() const
{
    // creates and returns an index buffer for given mesh
    auto createIndexBuffer = [] (const aiMesh* mesh) -> std::tuple<Ref<Buffer>, u32> {
        Vector<u32> indices;

        // iterate over all faces, which contain indices
        for (i32 i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (i32 j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return {
            create_ref<Buffer>(indices.data(), indices.size() * sizeof(u32), BufferUsage::Static),
            indices.size()
        };
    };

    // creates and returns a vertex buffer for given mesh
    auto createVertexBuffer = [] (const aiMesh* mesh) -> Ref<Buffer> {
        VertexBufferBuilder vbb{ Renderer().GetPBRPipeline()->GetLayout() };

        for (i32 i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& pos  = mesh->mVertices[i];
            const aiVector3D& norm = mesh->mNormals[i];
            const aiVector3D& tan  = mesh->mTangents[i];
            const aiVector3D& btan = mesh->mBitangents[i];
            const aiVector3D& uv   = mesh->mTextureCoords[0][i]; // only support 1 texture uv attr
            vbb.PushVertex(
                {
                    .position = { pos.x, pos.y, pos.z },
                    .normal = { norm.x, norm.y, norm.z },
                    .tangent = { tan.x, tan.y, tan.z },
                    .bitangent = { btan.x, btan.y, btan.z },
                    .texture = { uv.x, uv.y }
                }
            );
        }

        return vbb.Build();
    };

    // recursive function to traverse and load nodes of the mesh
    std::function<void(const aiNode*, const glm::mat4&)> traverseNode =
            [&] (const aiNode* node, const glm::mat4& parentTransform) -> void {
        // get this nodes transform
        const glm::mat4 transform = parentTransform * aiMatrixToGlm(node->mTransformation);

        // iterate over all meshes of this node and create surfaces
        for (i32 i = 0; i < node->mNumMeshes; i++) {
            // get the relevant mesh
            const aiMesh* mesh = m_scene->mMeshes[node->mMeshes[i]];

            // create buffers and geometry
            const auto [indexBuffer, indexCount] = createIndexBuffer(mesh);
            const Ref<Buffer> vertexBuffer       = createVertexBuffer(mesh);

            // fetch other surface related data
            const AssetHandle materialHandle = m_materials[mesh->mMaterialIndex];

            m_mesh->AddSurface(
                {
                    .transform = transform,
                    .materialHandle = materialHandle,
                    .vertices = vertexBuffer,
                    .indices = indexBuffer,
                    .indexCount = indexCount,
                }
            );
        }

        for (i32 i = 0; i < node->mNumChildren; i++) {
            traverseNode(node->mChildren[i], transform);
        }
    };

    traverseNode(m_scene->mRootNode, { 1 });
}
} // namespace siren::core
