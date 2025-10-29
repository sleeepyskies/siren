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

MeshImporter MeshImporter::create(const Path& path, const ImportContext context)
{
    return MeshImporter{ path, context };
}

MeshImporter& MeshImporter::defaults()
{
    return triangulate().generateNormals().calculateTangentSpace().optimizeMeshes().cleanMeshes();
}

MeshImporter& MeshImporter::triangulate()
{
    m_postProcessFlags |= aiProcess_Triangulate;
    return *this;
}

MeshImporter& MeshImporter::generateNormals()
{
    m_postProcessFlags |= aiProcess_GenNormals;
    return *this;
}

MeshImporter& MeshImporter::calculateTangentSpace()
{
    m_postProcessFlags |= aiProcess_CalcTangentSpace;
    return *this;
}

MeshImporter& MeshImporter::optimizeMeshes()
{
    m_postProcessFlags |= aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality;
    return *this;
}

MeshImporter& MeshImporter::cleanMeshes()
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

Ref<Mesh> MeshImporter::load()
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

    m_mesh = createRef<Mesh>(name);

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
        auto material = createRef<Material>(name);

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
        // metallic
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

        // alpha mode
        {
            float cutoff;
            float opacity;
            aiString alphaMode;

            if (aiMat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, cutoff) == AI_SUCCESS) {
                material->alphaCutoff = cutoff;
                material->alphaMode   = Material::AlphaMode::MASK;
            }

            if (aiMat->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode) == AI_SUCCESS) {
                // gltf specific
                std::string mode = alphaMode.C_Str();
                if (mode == "OPAQUE") {
                    material->alphaMode = Material::AlphaMode::OPAQUE;
                } else if (mode == "MASK") {
                    material->alphaMode = Material::AlphaMode::MASK;
                } else if (mode == "BLEND") {
                    material->alphaMode = Material::AlphaMode::BLEND;
                }
            } else if (aiMat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS && opacity < 1.f) {
                // else, try to infer
                material->alphaMode = Material::AlphaMode::BLEND;
            }
        }

        // ========= Render Flags =========

        // double-sided
        {
            bool doubleSided;
            if (aiMat->Get(AI_MATKEY_TWOSIDED, doubleSided) == AI_SUCCESS) {
                material->doubleSided = doubleSided;
            }
        }

        // ========= Textures =========

        auto loadTexture = [&] (
            const aiTextureType aiTextureType,
            const Material::TextureType sirenTextureType
        ) -> void {
            aiString texturePath;
            if (aiMat->GetTexture(aiTextureType, 0, &texturePath) != AI_SUCCESS) {
                return;
            }

            auto textureImporter   = TextureImporter::create(m_scene, texturePath);
            Ref<Texture2D> texture = textureImporter.load();

            if (!texture) {
                return;
            }

            const AssetHandle textureHandle = AssetHandle::create();
            const AssetMetaData metaData{
                .type = AssetType::TEXTURE2D,
                .sourceData = materialHandle,
            };

            if (m_context.registerAsset(textureHandle, texture, metaData)) {
                material->setTexture(Material::TextureType::BASE_COLOR, textureHandle);
                return;
            }

            dbg("Invalid Texture parsed. Cannot create mesh.");
            m_success = false;
        };

        // base color
        loadTexture(aiTextureType_BASE_COLOR, Material::TextureType::BASE_COLOR);
        // metallic roughness
        loadTexture(aiTextureType_AMBIENT_OCCLUSION, Material::TextureType::OCCLUSION);
        if (!material->hasTexture(Material::TextureType::OCCLUSION)) {
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
        loadTexture(aiTextureType_NORMALS, Material::TextureType::NORMAL);
        // emission
        loadTexture(aiTextureType_EMISSION_COLOR, Material::TextureType::EMISSION);
        // occlusion
        loadTexture(aiTextureType_AMBIENT_OCCLUSION, Material::TextureType::OCCLUSION);

        // shader
        {
            // not clean to call assets here but whatever
            const auto result = assets().createShader(material->getMaterialKey());
            if (!result) {
                dbg("Cannot assign material a null shader.");
                m_success = false;
                return;
            }

            material->shaderHandle = *result;
        }

        AssetMetaData metaData{
            .type = AssetType::MATERIAL,
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
    u32 meshCount = 0;

    // creates and returns an index buffer for given mesh
    auto createIndexBuffer = [] (const aiMesh* mesh) -> Ref<IndexBuffer> {
        std::vector<u32> indices;

        // iterate over all faces, which contain indices
        for (i32 i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (i32 j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return createRef<IndexBuffer>(indices);
    };

    // creates and returns a vertex buffer for given mesh
    auto createVertexBuffer = [] (const aiMesh* mesh) -> Ref<VertexBuffer> {
        VertexData data{ };

        for (i32 i = 0; i < mesh->mNumVertices; ++i) {
            // position
            const aiVector3D& pos = mesh->mVertices[i];
            data.positions.push_back(glm::vec3(pos.x, pos.y, pos.z));
            // normal
            const aiVector3D& norm = mesh->mNormals[i];
            data.normals.push_back(glm::vec3(norm.x, norm.y, norm.z));
            // tangent
            const aiVector3D& tan = mesh->mTangents[i];
            data.tangents.push_back(glm::vec3(tan.x, tan.y, tan.z));
            // bitangent
            const aiVector3D& btan = mesh->mBitangents[i];
            data.bitangents.push_back(glm::vec3(btan.x, btan.y, btan.z));
            // uvs
            const aiVector3D& uv = mesh->mTextureCoords[0][i]; // only support 1 texture uv attr
            data.textureUvs.push_back(glm::vec2(uv.x, uv.y));
            // color (optional) only support 1 color attr
            // fixme: do we need to check this each loop?
            if (mesh->HasVertexColors(0)) {
                const aiColor4D& col = mesh->mColors[0][i];
                data.normals.push_back(glm::vec4(col.r, col.g, col.b, col.a));
            }
        }

        return createRef<VertexBuffer>(data, BufferUsage::STATIC);
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
            const Ref<IndexBuffer> indexBuffer   = createIndexBuffer(mesh);
            const Ref<VertexBuffer> vertexBuffer = createVertexBuffer(mesh);
            const Ref<VertexArray> vertexArray   = createRef<VertexArray>();
            vertexArray->linkIndexBuffer(indexBuffer);
            vertexArray->linkVertexBuffer(vertexBuffer);

            // fetch other surface related data
            const AssetHandle materialHandle = m_materials[mesh->mMaterialIndex];

            m_mesh->addSurface(
                {
                    .transform = transform,
                    .materialHandle = materialHandle,
                    .vertexArray = vertexArray
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
