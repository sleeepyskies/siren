#include "ModelImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include "assets/Asset.hpp"
#include "core/Application.hpp"
#include "geometry/Material.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/Buffer.hpp"
#include "utilities/SirenAssert.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{
static glm::vec4 defaultColor     = { 0.6f, 0.1f, 0.9f, 1.0f }; // purple
static glm::vec3 defaultNorm      = { 0.0f, 0.0f, 1.0f };
static glm::vec3 defaultTangent   = { 0.0f, 1.0f, 0.0f };
static glm::vec3 defaultBitangent = { 1.0f, 0.0f, 0.0f };
static glm::vec2 defaultTextureUV = { 0.0f, 0.0f };

static uint32_t importCount = 0; // used for default unique mesh name
static uint32_t matCount    = 0; // used for default unique material name

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

static Ref<renderer::Texture2D> loadTexture(const std::string& path, aiTexture** textures)
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
        int width, height, channels;
        stbi_uc* rawData = stbi_load(path.c_str(), &width, &height, &channels, STBI_default);

        if (!rawData) { return nullptr; }

        std::vector<Byte> data(rawData, rawData + (width * height * channels));
        stbi_image_free(rawData);

        renderer::Image2DSampler imageSampler{};

        return makeRef<renderer::Texture2D>(data, imageSampler, width, height);
    }
}

// TODO: the attributes this retrieves are by no means final, is just based on gltf
static Ref<geometry::Material> loadMaterial(const aiMaterial* aiMat, aiTexture** textures)
{
    const std::string name = !aiMat->GetName().Empty() ? std::string(aiMat->GetName().C_Str())
                                                       : "Material_" + std::to_string(matCount++);
    auto material = makeRef<geometry::Material>(/* name */); // TODO: make Material an asset

    // required valus with default values
    // baseColorFactor
    {
        aiColor3D color;
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->baseColorFactor = glm::vec4(color.r, color.g, color.b, 1);
    }
    // metallicFactor
    {
        float metallic;
        aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
        material->metallicFactor = metallic;
    }
    // roughnessFactor
    {
        float roughness;
        aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
        material->roughnessFactor = roughness;
    }
    // emissionColor
    {
        aiColor3D color;
        aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        material->emissionColor = glm::vec4(color.r, color.g, color.b, 1);
    }
    // occlusionStength
    {
        // HACK: material is still gltf based, and Assimp doesnt provide this directly
        material->occlusionStrength = 1;
    }
    // occlusionStength
    {
        // HACK: material is still gltf based, and Assimp doesnt provide this directly
        material->normalScale = 1;
    }

    aiString path;

    // optional textures
    // baseColorMap
    if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        material->baseColorMap = loadTexture(std::string(path.C_Str()), textures);
    }
    // metallicMap
    if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS) {
        material->metallicMap = loadTexture(std::string(path.C_Str()), textures);
    }
    // roughnessMap
    if (aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS) {
        material->roughnessMap = loadTexture(std::string(path.C_Str()), textures);
    }
    // emissionMap
    if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
        material->emissionMap = loadTexture(std::string(path.C_Str()), textures);
    }
    // occlusionMap
    if (aiMat->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS) {
        material->occlusionMap = loadTexture(std::string(path.C_Str()), textures);
    }
    // normalMap
    if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
        material->normalMap = loadTexture(std::string(path.C_Str()), textures);
    }

    return material;
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
        if (aiMesh->HasVertexColors(0) && aiMesh->mColors && aiMesh->mColors[0]) {
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

Ref<geometry::Model> ModelImporter::importModel(const fs::path& path)
{
    if (!exists(path)) {
        err("File does not exist at {}", path.string());
        return nullptr;
    }

    // NOTE:
    // this function assumes there will only be one model per file. If a file contains
    // multiple models inside, they will all be treated as one model.
    Assimp::Importer importer;

    unsigned int postProcessFlags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
                                    aiProcess_GenNormals | aiProcess_CalcTangentSpace |
                                    aiProcess_GenUVCoords;
    const aiScene* scene = importer.ReadFile(path.string(), postProcessFlags);

    if (!scene) {
        wrn("Failed to load model from {}", path.string());
        return nullptr;
    }

    const std::string name     = !scene->mName.Empty() ? std::string(scene->mName.C_Str())
                                                       : "Model_" + std::to_string(importCount++);
    Ref<geometry::Model> model = makeRef<geometry::Model>(name);

    std::vector<Ref<geometry::Material>> materials{};
    for (int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* aiMaterial = scene->mMaterials[i];
        materials.push_back(loadMaterial(scene->mMaterials[i], scene->mTextures));
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
            // siren can only handle tris atm, we triangulate so no polygons but lines and points
            // remain
            if (!(aiMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
                wrn("Could not import model as it does not consist of tris");
                return nullptr;
            }

            auto va = makeRef<renderer::VertexArray>();

            // assimp produces unsigned int indices only

            // HACK: Buffers were made to be type agnostic due to low level importers, but assimp
            // only provides fixed types for most cases, so we dont actaully need to copy the whole
            // raw index buffer

            // index buffer
            {
                renderer::IndexDataType indexType{ renderer::IndexDataType::INDEX_UINT };
                auto ib = makeRef<renderer::IndexBuffer>(indexType);
                std::vector<uint32_t> indices{};
                for (int faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++) {
                    aiFace& aiFace = aiMesh->mFaces[faceIndex];
                    for (int indexIndex = 0; indexIndex < aiFace.mNumIndices; indexIndex++) {
                        indices.push_back(aiFace.mIndices[indexIndex]);
                    }
                }
                std::vector<Byte> indexBytes(indices.size() * sizeof(uint32_t));
                std::memcpy(indexBytes.data(), indices.data(), indices.size() * sizeof(uint32_t));
                ib->uploadIndices(indexBytes, renderer::BufferUsage::STATIC);
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
            layout.addVertexAttribute({ "position", 3, GL_FLOAT });  // required
            layout.addVertexAttribute({ "normal", 3, GL_FLOAT });    // required
            layout.addVertexAttribute({ "tangent", 3, GL_FLOAT });   // required
            layout.addVertexAttribute({ "bitangent", 3, GL_FLOAT }); // required
            layout.addVertexAttribute({ "textureuv", 2, GL_FLOAT }); // required
            layout.addVertexAttribute({ "color", 4, GL_FLOAT });     // defaults to purple
            layout.close();

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
        }
    }

    return model;
}

} // namespace siren::assets
