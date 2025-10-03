// ReSharper disable CppTooWideScopeInitStatement
#include "ModelImporter.hpp"

#include <core/Application.hpp>
#include <tiny_gltf.h>

namespace core::assets::ModelImporter
{
using ImportModelFn = std::function<Ref<geometry::Model>(const fs::path&)>;
static std::unordered_map<std::string, ImportModelFn> extensionToImportFn = {
    // gltf files
    { ".gltf", importModelFromGLTF },
    { ".glb", importModelFromGLTF },
    // obj
    { ".obj", importModelFromOBJ },
};

// must be same order as in uber shader
static std::vector<std::string> allowedGltfAttributes = {
    "POSITION",
    "COLOR_0",
    "NORMAL",
    "TEXCOORD_0",
};

Ref<geometry::Model> importModel(const fs::path& path)
{
    if (!exists(path)) {
        err("File does not exist at {}", path.string());
        return nullptr;
    }

    const std::string extension = path.extension().string();
    if (!extensionToImportFn.contains(extension)) {
        err("File with invalid extension provided: {}", path.string());
        return nullptr;
    }

    return extensionToImportFn[extension](path);
}

// TODO: this loader needs to be written again making use of other importers as well as taking gltf
// nodes into account
Ref<geometry::Model> importModelFromGLTF(const fs::path& path)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool success;

    if (path.extension() == ".gltf")
        success = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path.string());
    else
        success = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path.string());

    if (!warn.empty()) {
        wrn("{}", warn);
        return nullptr;
    }

    if (!err.empty()) {
        err("{}", err);
        return nullptr;
    }

    if (!success) {
        err("Could not load model at {}", path.string());
        return nullptr;
    }

    Ref<geometry::Model> model = makeRef<geometry::Model>(gltfModel.meshes[0].name, glm::mat4(1));
    std::vector<Ref<renderer::Texture2D>> textures{};
    std::vector<Ref<geometry::Material>> materials{};

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
            gltfImage.image, sampler, gltfImage.width, gltfImage.height));
    }

    // TODO: support multiple Texture UV coordinates for different material components
    for (const auto& gltfMaterial : gltfModel.materials) {
        Ref<geometry::Material> material = makeRef<geometry::Material>();

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

            material->occlusionStrength = ot.strength;
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
                SirenAssert(gltfModel.accessors[attribute.second].count == vertexCount,
                            "Invalid gltf file. All primitive attributes must have the "
                            "same length.");
            }

            // primitives describe a submesh and get a unique VAO
            Ref<renderer::VertexArray> vao = makeRef<renderer::VertexArray>();

            // optional IndexBuffer
            if (prim.indices != -1) {
                const tinygltf::Accessor& accessor     = gltfModel.accessors[prim.indices];
                const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer         = gltfModel.buffers[bufferView.buffer];
                const renderer::IndexDataType indexType{ accessor.componentType };
                Ref<renderer::IndexBuffer> ebo = makeRef<renderer::IndexBuffer>(indexType);

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

            // we want to rearrange all attributes of this primitive into one vertex
            // buffer
            std::vector<Byte> data{};
            renderer::VertexBufferLayout bufferLayout{};
            std::vector<uint32_t> offsets{};

            for (const auto& name : allowedGltfAttributes) {
                if (!prim.attributes.contains(name)) continue;
                const int accIndex                = prim.attributes.at(name);
                const tinygltf::Accessor accessor = gltfModel.accessors[accIndex];
                const int count                   = name == "COLOR_0" ? 4 : accessor.type;
                const renderer::AttributeDataType attributeType{ accessor.componentType, count };
                bufferLayout.addVertexAttribute(
                    { name, attributeType.count, attributeType.dataType() });
                offsets.push_back(0);
            }
            bufferLayout.close();

            for (int i = 0; i < vertexCount; i++) {
                for (int j = 0; j < allowedGltfAttributes.size(); j++) {
                    const std::string& name = allowedGltfAttributes[j];

                    // only use certain attributes so fill in dummy data
                    if (!prim.attributes.contains(allowedGltfAttributes[j])) {
                        std::vector<float> dummy;
                        if (name == "COLOR_0") {
                            dummy = { 0, 0, 0, 1 }; // always vec4
                        } else if (name == "NORMAL") {
                            dummy = { 0, 0, 0 };
                        } else if (name == "TEXCOORD_0") {
                            dummy = { 0, 0 };
                        }
                        const uint32_t size = dummy.size() * sizeof(float);
                        copyToBuffer(data, dummy, 0, size);
                        offsets[j] += size;
                        continue;
                    }
                    const tinygltf::Accessor& accessor =
                        gltfModel.accessors[prim.attributes.at(name)];
                    const tinygltf::BufferView& bufferView =
                        gltfModel.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

                    const renderer::AttributeDataType attributeType{ accessor.componentType,
                                                                     accessor.type };

                    size_t components = accessor.type;
                    size_t vertexOffset =
                        bufferView.byteOffset + accessor.byteOffset + i * bufferView.byteStride;

                    copyToBuffer(
                        data, buffer.data, vertexOffset, vertexOffset + components * sizeof(float));
                    offsets[j] += components * sizeof(float);

                    // enforce color is vec4
                    if (name == "COLOR_0" && accessor.type == 3) {
                        float alpha = 1.f;
                        copyToBuffer(data, std::vector<float>{ alpha }, 0, sizeof(float));
                        offsets[j] += sizeof(float);
                    }
                }
            }

            Ref<renderer::VertexBuffer> vbo =
                makeRef<renderer::VertexBuffer>(data, renderer::BufferUsage::STATIC, bufferLayout);
            vao->linkVertexBuffer(vbo);

            const Ref<geometry::Material>& mat =
                (prim.material != -1) ? materials[prim.material] : nullptr;

            model->addMesh({ mat, vao, { 1 } });
        }
    }

    return model;
}

inline Ref<geometry::Model> importModelFromOBJ(const fs::path& path)
{
    wrn("Not implemented");
    return nullptr;
}

} // namespace core::assets::ModelImporter
