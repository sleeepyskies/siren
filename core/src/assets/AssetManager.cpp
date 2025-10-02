#include "AssetManager.hpp"

#include "renderer/Texture.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include "tiny_gltf.h"

namespace core::assets
{

AssetManager::AssetManager()
{
    // pass
}

AssetManager::~AssetManager()
{
    // pass
}

void AssetManager::init(const fs::path& workingDirectory)
{
    m_workingDirectory = workingDirectory;
    m_assetDirectory   = workingDirectory / "assets";
    m_modelDirectory   = m_assetDirectory / "models";
}

void AssetManager::shutdown()
{
    // unload all assets?
}

ModelID AssetManager::loadModelFromRelativePath(const fs::path& relativePath)
{
    SirenAssert(m_workingDirectory != "", "There is no working directory to load models from");
    SirenAssert(m_assetDirectory != "", "There is no asset directory to load models from");

    if (relativePath.extension().string() != ".gltf" &&
        relativePath.extension().string() != ".glb") {
        err("Invalid file provided for loading {}", relativePath.string());
        return 0;
    }

    if (!fs::exists(relativePath)) {
        err("Invalid file provided for loading {}", relativePath.string());
        return 0;
    }

    const ModelID modelHash = hashModelPath(relativePath.string());

    // if loaded, just return hash
    if (m_loadedModels.contains(modelHash)) return modelHash;

    // if not loaded, load and then return hash
    const ref<geometry::Mesh> model = loadModelFromAbsolutePath(m_workingDirectory / relativePath);
    if (!model) {
        err("Could not parse or construct model from given file {}", relativePath.string());
        return 0;
    }
    m_loadedModels[modelHash] = model;

    nfo("Successfully loaded model at {}", relativePath.string());
    return modelHash;
}

ModelID AssetManager::hashModelPath(const std::string& model) const
{
    return std::hash<std::string>{}(model);
}

ref<geometry::Mesh> AssetManager::getModelByID(const ModelID id) const
{
    if (m_loadedModels.contains(id)) return m_loadedModels.at(id);
    return nullptr;
}

ref<geometry::Mesh> AssetManager::loadModelFromAbsolutePath(const fs::path& absolutePath)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool success;

    if (absolutePath.extension() == ".gltf")
        success = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, absolutePath.string());
    else
        success = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, absolutePath.string());

    if (!warn.empty()) {
        wrn("{}", warn);
        return nullptr;
    }

    if (!err.empty()) {
        err("{}", err);
        return nullptr;
    }

    if (!success) {
        err("Could not load model at {}", absolutePath.string());
        return nullptr;
    }

    ref<geometry::Mesh> mesh = makeRef<geometry::Mesh>();
    std::vector<ref<renderer::Texture2D>> textures{};
    std::vector<ref<geometry::Material>> materials{};

    // TODO: better caching system for images and textures. Currently do not reuse unless all
    // withing a single gltf file. Would need to cache results based on URI. This should also be
    // consistent with front facing api for loading images/textures.
    for (const auto& gltfTexture : gltfModel.textures) {
        const tinygltf::Image& gltfImage     = gltfModel.images[gltfTexture.source];
        const tinygltf::Sampler& gltfSampler = gltfModel.samplers[gltfTexture.sampler];

        renderer::Image2DSampler sampler;
        sampler.magnification =
            static_cast<renderer::Image2DSampler::Filtering>(gltfSampler.magFilter);
        sampler.minification =
            static_cast<renderer::Image2DSampler::Filtering>(gltfSampler.minFilter);
        sampler.sWrap = static_cast<renderer::Image2DSampler::WrapMode>(gltfSampler.wrapS);
        sampler.tWrap = static_cast<renderer::Image2DSampler::WrapMode>(gltfSampler.wrapT);

        textures.emplace_back(makeRef<renderer::Texture2D>(
            gltfImage.image, gltfImage.name, sampler, gltfImage.width, gltfImage.height));
    }

    // TODO: support multiple Texture UV coordinates for different material components
    for (const auto& gltfMaterial : gltfModel.materials) {
        ref<geometry::Material> material = makeRef<geometry::Material>(gltfMaterial.name);

        // Base color
        {
            const std::vector<double>& bcf   = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
            const tinygltf::TextureInfo& bct = gltfMaterial.pbrMetallicRoughness.baseColorTexture;

            material->baseColorFactor = glm::vec4(bcf[0], bcf[1], bcf[2], bcf[3]);
            if (bct.index != -1) material->baseColorMap = textures[bct.index];
        }

        // Metallic Roughness
        {
            const tinygltf::TextureInfo& mrt =
                gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture;

            material->metallicFactor  = gltfMaterial.pbrMetallicRoughness.metallicFactor;
            material->roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
            if (mrt.index != -1) material->metallicRoughnessMap = textures[mrt.index];
        }

        // Emission
        {
            const std::vector<double>& ef   = gltfMaterial.emissiveFactor;
            const tinygltf::TextureInfo& et = gltfMaterial.emissiveTexture;

            material->emissionColor = glm::vec3(ef[1], ef[1], ef[2]);
            if (et.index != -1) material->emissionMap = textures[et.index];
        }

        // Occlusion
        {
            const tinygltf::OcclusionTextureInfo& ot = gltfMaterial.occlusionTexture;

            material->oclussionStength = ot.strength;
            if (ot.index != -1) material->occlusionMap = textures[ot.index];
        }

        // Normals
        {
            const tinygltf::NormalTextureInfo& nt = gltfMaterial.normalTexture;

            material->normalScale = nt.scale;
            if (nt.index != -1) material->normalsMap = textures[nt.index];
        }

        materials.push_back(material);
    }

    for (const auto& m : gltfModel.meshes) {
        // a mesh describes a whole cohesive object,
        for (const auto& prim : m.primitives) {
            // all attributes within a primitive must have the same vertex count
            const size_t vertexCount = gltfModel.accessors[prim.attributes.begin()->second].count;
            for (const auto& attribute : prim.attributes) {
                SirenAssert(
                    gltfModel.accessors[attribute.second].count == vertexCount,
                    "Invalid gltf file. All primitive attributes must have the same length.");
            }

            // primitives describe a submesh and get a unique VAO
            ref<renderer::VertexArray> vao = makeRef<renderer::VertexArray>();

            // optional IndexBuffer
            if (prim.indices != -1) {
                const tinygltf::Accessor& accessor     = gltfModel.accessors[prim.indices];
                const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer         = gltfModel.buffers[bufferView.buffer];
                const renderer::IndexDataType indexType{ accessor.componentType };
                ref<renderer::IndexBuffer> ebo = makeRef<renderer::IndexBuffer>(indexType);

                std::vector<Byte> indexData{};
                // we assume type aka vec1/2/3/4 is always 1
                copyToBufferWithStride(indexData,
                                       buffer.data,
                                       bufferView.byteOffset + accessor.byteOffset,
                                       bufferView.byteOffset + bufferView.byteLength,
                                       bufferView.byteStride,
                                       indexType.size());

                ebo->uploadIndices(indexData, renderer::BufferUsage::STATIC);
                vao->linkIndexBuffer(ebo);
            }

            // optional material

            // we want to rearrange all attributes of this primitive into one vertex buffer
            std::vector<Byte> data{};
            renderer::VertexBufferLayout bufferLayout{};
            std::vector<uint32_t> offsets(prim.attributes.size(), 0); // set n zeros
            std::vector<std::string> names{};

            for (const auto& [name, accIndex] : prim.attributes) {
                names.push_back(name);
                const tinygltf::Accessor accessor = gltfModel.accessors[accIndex];
                const renderer::AttributeDataType attributeType{ accessor.componentType,
                                                                 accessor.type };
                bufferLayout.addVertexAttribute(
                    { name, attributeType.count, attributeType.dataType() });
            }
            bufferLayout.close();

            // FIXME: redefinition of i variable here!!
            for (int i = 0; i < vertexCount; i++) {
                for (int i = 0; i < prim.attributes.size(); i++) {
                    const std::string& name = names[i];
                    const tinygltf::Accessor& accessor =
                        gltfModel.accessors[prim.attributes.at(names[i])];
                    const tinygltf::BufferView& bufferView =
                        gltfModel.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];
                    const renderer::AttributeDataType attributeType{ accessor.componentType,
                                                                     accessor.type };

                    copyToBuffer(data,
                                 buffer.data,
                                 offsets[i] + bufferView.byteOffset + accessor.byteOffset,
                                 offsets[i] + bufferView.byteOffset + accessor.byteOffset +
                                     attributeType.size());

                    offsets[i] += bufferView.byteStride;
                }
            }

            ref<renderer::VertexBuffer> vbo = makeRef<renderer::VertexBuffer>();
            vbo->uploadData(data, renderer::BufferUsage::STATIC);
            vao->linkVertexBuffer(vbo, bufferLayout);

            const ref<geometry::Material>& mat =
                (prim.material != -1) ? materials[prim.material] : nullptr;
            mesh->addSubmesh({ mat, vao });
        }
    }

    return mesh;
}

} // namespace core::assets
