#include "ModelImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include "assets/Asset.hpp"
#include "core/Application.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/buffer/IndexBuffer.hpp"
#include "renderer/buffer/VertexArray.hpp"
#include "renderer/buffer/VertexBuffer.hpp"
#include "renderer/material/Material.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{
static glm::vec4 defaultColor     = { 0.6f, 0.1f, 0.9f, 1.0f }; // purple
static glm::vec3 defaultNorm      = { 0.0f, 0.0f, 1.0f };
static glm::vec3 defaultTangent   = { 0.0f, 1.0f, 0.0f };
static glm::vec3 defaultBitangent = { 1.0f, 0.0f, 0.0f };
static glm::vec2 defaultTextureUV = { 0.0f, 0.0f };

static uint32_t s_importCount = 0; // used for default unique mesh name
static uint32_t s_matCount    = 0; // used for default unique material name

struct NodeElem {
    aiNode* node;
    glm::mat4 transform{};
};

glm::mat4 aiMatrixToGlm(const aiMatrix4x4& m)
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

static Ref<renderer::Texture2D> loadTexture(const std::string& path, aiTexture** textures,
                                            const Path& modelDirectory)
{
    if (path.starts_with('*')) {
        // some formats have embedded textures, in which case the path is *n
        const int index            = std::stoi(path.substr(1));
        const aiTexture* aiTexture = textures[index];

        renderer::Image2DSampler imageSampler{};
        std::vector<Byte> data{};

        const int width  = aiTexture->mWidth;
        const int height = aiTexture->mHeight;

        if (height == 0) {
            // we have compressed data
            const auto compressedData = reinterpret_cast<const stbi_uc*>(aiTexture->pcData);
            int w, h, c;
            stbi_uc* rawData =
                stbi_load_from_memory(compressedData, width, &w, &h, &c, STBI_default);

            data = std::vector<Byte>(rawData, rawData + (w * h * c));
            stbi_image_free(rawData);

        } else {
            // we non compressed data
            for (int texelIndex = 0; texelIndex < width * height; texelIndex++) {
                const aiTexel& aiTexel = aiTexture->pcData[texelIndex];
                data.push_back(aiTexel.r);
                data.push_back(aiTexel.g);
                data.push_back(aiTexel.b);
                data.push_back(aiTexel.a);
            }
        }

        return makeRef<renderer::Texture2D>(data, imageSampler, width, height);
    } else {
        // otherwise, we have a default path to file
        const Path imagePath = modelDirectory / path;
        int width, height, channels;
        stbi_uc* rawData =
            stbi_load(imagePath.string().c_str(), &width, &height, &channels, STBI_default);

        if (!rawData) { return nullptr; }

        std::vector<Byte> data(rawData, rawData + (width * height * channels));
        stbi_image_free(rawData);

        renderer::Image2DSampler imageSampler{};

        return makeRef<renderer::Texture2D>(data, imageSampler, width, height);
    }
}

// TODO: the attributes this retrieves are by no means final, is just based on gltf
static Maybe<AssetHandle> loadMaterial(const aiMaterial* aiMat, aiTexture** textures,
                                       const Path& filePath)
{
    const Path modelDirectory = filePath.parent_path();

    const std::string name = !aiMat->GetName().Empty() ? std::string(aiMat->GetName().C_Str())
                                                       : "Material_" + std::to_string(s_matCount++);
    auto material          = makeRef<renderer::Material>(name);

    // required values with default values
    // baseColorFactor
    {
        // HACK: since we have no robust shader system yet, we just pick either diffuse or base
        // color for color and treat them the same
        aiColor3D color(1.0f, 1.0f, 1.0f);
        if (aiMat->Get(AI_MATKEY_BASE_COLOR, color) != AI_SUCCESS) {
            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        }
        material->baseColor = glm::vec4(color.r, color.g, color.b, 1.0f);
    }
    // metallic
    {
        float metallic;
        aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
        material->metallic = metallic;
    }
    // roughness
    {
        float roughness;
        aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
        material->roughness = roughness;
    }
    // emission
    {
        aiColor3D color;
        aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        material->emission = glm::vec4(color.r, color.g, color.b, 1);
    }
    // occlusion
    {
        // HACK: material is still gltf based, and Assimp doesn't provide this directly
        material->occlusion = 1;
    }
    // normal
    {
        // HACK: material is still gltf based, and Assimp doesn't provide this directly
        material->normalScale = 1;
    }

    aiString path;

    // optional textures
    // baseColorMap
    if (aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::BASE_COLOR);
    } else if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::BASE_COLOR);
    }
    // metallicMap
    if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::METALLIC);
    }
    // roughnessMap
    if (aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::ROUGHNESS);
    }
    // emissionMap
    if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::EMISSION);
    }
    // occlusionMap
    if (aiMat->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::OCCLUSION);
    }
    // normalMap
    if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
        const auto texture = loadTexture(std::string(path.C_Str()), textures, modelDirectory);
        material->pushTexture(texture, renderer::TextureType::NORMAL);
    }
    // load shader
    const auto shaderRes =
        core::Application::get().getShaderManager().loadShader(material->generateMaterialKey());
    if (!shaderRes) { return Nothing; }
    material->shaderHandle = *shaderRes;

    // register material

    const AssetHandle materialHandle{};
    // assimp doesn't provide a material path, so we just assign incremental virtual paths
    const Path virtualPath = filePath / ("Material_" + std::to_string(s_matCount));
    core::Application::get().getAssetRegistry().registerAsset(
        materialHandle, material, virtualPath, true);

    return materialHandle;
}

static std::vector<Byte> loadVertexData(const aiMesh* aiMesh)
{
    const int numUVComponents = aiMesh->mNumUVComponents[0];
    SirenAssert(numUVComponents == 2 || numUVComponents == 0,
                "Only 2D UV-Coordinates are supported");

    std::vector<Byte> data{};

    for (int vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++) {
        // position
        {
            const aiVector3D& pos = aiMesh->mVertices[vertexIndex];
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&pos),
                        reinterpret_cast<const Byte*>(&pos) + sizeof(pos));
        }

        // normal (always write)
        if (aiMesh->mNormals) {
            const aiVector3D& n = aiMesh->mNormals[vertexIndex];
            glm::vec3 norm{ n.x, n.y, n.z };
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&norm),
                        reinterpret_cast<const Byte*>(&norm) + sizeof(norm));
        } else {
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&defaultNorm),
                        reinterpret_cast<const Byte*>(&defaultNorm) + sizeof(defaultNorm));
        }

        // tangent (always write)
        if (aiMesh->mTangents) {
            const aiVector3D& t = aiMesh->mTangents[vertexIndex];
            glm::vec3 tan{ t.x, t.y, t.z };
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&tan),
                        reinterpret_cast<const Byte*>(&tan) + sizeof(tan));
        } else {
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&defaultTangent),
                        reinterpret_cast<const Byte*>(&defaultTangent) + sizeof(defaultTangent));
        }

        // bitangent (always write)
        if (aiMesh->mBitangents) {
            const aiVector3D& b = aiMesh->mBitangents[vertexIndex];
            glm::vec3 bit{ b.x, b.y, b.z };
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&bit),
                        reinterpret_cast<const Byte*>(&bit) + sizeof(bit));
        } else {
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&defaultBitangent),
                        reinterpret_cast<const Byte*>(&defaultBitangent) +
                            sizeof(defaultBitangent));
        }

        // UV (always write vec2)
        if (aiMesh->HasTextureCoords(0) && aiMesh->mTextureCoords[0]) {
            const aiVector3D& uv = aiMesh->mTextureCoords[0][vertexIndex];
            glm::vec2 uvt{ uv.x, uv.y };
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&uvt),
                        reinterpret_cast<const Byte*>(&uvt) + sizeof(uvt));
        } else {
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&defaultTextureUV),
                        reinterpret_cast<const Byte*>(&defaultTextureUV) +
                            sizeof(defaultTextureUV));
        }

        // color (always write vec4)
        if (aiMesh->HasVertexColors(0) && aiMesh->mColors) {
            const aiColor4D& c = aiMesh->mColors[0][vertexIndex];
            glm::vec4 col{ c.r, c.g, c.b, c.a };
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&col),
                        reinterpret_cast<const Byte*>(&col) + sizeof(col));
        } else {
            data.insert(data.end(),
                        reinterpret_cast<const Byte*>(&defaultColor),
                        reinterpret_cast<const Byte*>(&defaultColor) + sizeof(defaultColor));
        }
    }

    return data;
}

Ref<geometry::Model> ModelImporter::importModel(const Path& path)
{
    s_matCount = 0;

    if (!exists(path)) {
        err("File does not exist at {}", path.string());
        return nullptr;
    }

    // NOTE:
    // this function assumes there will only be one model per file. If a file contains
    // multiple models inside, they will all be treated as one model.
    Assimp::Importer importer;

    constexpr unsigned int postProcessFlags = aiProcess_JoinIdenticalVertices |
                                              aiProcess_Triangulate | aiProcess_GenNormals |
                                              aiProcess_CalcTangentSpace | aiProcess_GenUVCoords;
    const aiScene* scene = importer.ReadFile(path.string(), postProcessFlags);

    if (!scene) {
        wrn("Failed to load model from {}", path.string());
        return nullptr;
    }

    const std::string name     = !scene->mName.Empty() ? std::string(scene->mName.C_Str())
                                                       : "Model_" + std::to_string(s_importCount++);
    Ref<geometry::Model> model = makeRef<geometry::Model>(name);

    std::vector<AssetHandle> materials{};
    for (int i = 0; i < scene->mNumMaterials; i++) {
        const auto materialRes = loadMaterial(scene->mMaterials[i], scene->mTextures, path);
        if (!materialRes) { return nullptr; }
        materials.push_back(*materialRes);
    }

    if (!scene->mRootNode) { return nullptr; }
    if (!scene->mNumMeshes) { return nullptr; }

    AssetRegistry& assetRegistry = core::Application::get().getAssetManager().getAssetRegistry();

    std::stack<NodeElem> nodeStack{};
    nodeStack.emplace(scene->mRootNode, aiMatrixToGlm(scene->mRootNode->mTransformation));

    uint32_t meshCount = 0;

    // traverse nodes, making a new mesh for each node, carrying the transform through each level
    while (!nodeStack.empty()) {
        // setup stack
        const NodeElem elem       = nodeStack.top();
        const aiNode* node        = elem.node;
        const glm::mat4 transform = elem.transform;

        nodeStack.pop();

        for (int i = 0; i < node->mNumChildren; i++) {
            nodeStack.emplace(node->mChildren[i],
                              transform * aiMatrixToGlm(node->mChildren[i]->mTransformation));
        }

        // load actual mesh
        for (int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
            aiMesh* aiMesh = scene->mMeshes[node->mMeshes[meshIndex]];
            // siren can only handle tris
            if (!(aiMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
                wrn("Could not import model as it does not consist of tris");
                return nullptr;
            }

            auto va = makeRef<renderer::VertexArray>();

            // index buffer
            {
                std::vector<uint32_t> indices{};
                for (int faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++) {
                    aiFace& aiFace = aiMesh->mFaces[faceIndex];
                    for (int indexIndex = 0; indexIndex < aiFace.mNumIndices; indexIndex++) {
                        indices.push_back(aiFace.mIndices[indexIndex]);
                    }
                }
                auto ib = makeRef<renderer::IndexBuffer>(indices);
                va->linkIndexBuffer(ib);
            }

            // NOTE:
            // We require tangent and bitangent since normal maps are stored in tangent space,
            // not world space. So they are in NTB space. This means that any rotations or
            // deformations that affect an object will not mess up the normals.

            std::vector<Byte> vertexData = loadVertexData(aiMesh);

            // NOTE: siren expects this order always at the moment dynamic shaders from a
            // ShaderManager would be nice at some point...
            renderer::VertexBufferLayout layout{};
            layout.addVertexAttribute(renderer::ShaderAttribute::POSITION);  // required
            layout.addVertexAttribute(renderer::ShaderAttribute::NORMAL);    // required
            layout.addVertexAttribute(renderer::ShaderAttribute::TANGENT);   // required
            layout.addVertexAttribute(renderer::ShaderAttribute::BITANGENT); // required
            layout.addVertexAttribute(renderer::ShaderAttribute::TEXTUREUV); // required
            layout.addVertexAttribute(renderer::ShaderAttribute::COLOR);     // optional

            auto vbo =
                makeRef<renderer::VertexBuffer>(vertexData, renderer::BufferUsage::STATIC, layout);
            va->linkVertexBuffer(vbo);
            auto mesh = makeRef<geometry::Mesh>(std::string(aiMesh->mName.C_Str()),
                                                materials[aiMesh->mMaterialIndex],
                                                va,
                                                transform);
            AssetHandle meshHandle{};
            assetRegistry.registerAsset(
                meshHandle, mesh, path / ("Model_" + std::to_string(meshCount++)), true);
            model->addMesh(meshHandle);
            // dbg("Mesh {} has material {}", mesh->getName(), mesh->getMaterialHandle());
        }
    }

    return model;
}

} // namespace siren::assets
