#include "GLTFImporter.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <tiny_gltf.h>

namespace siren::assets
{

// must be same order as in uber shader
static std::vector<std::string> allowedGltfAttributes = { "POSITION", //
                                                          "COLOR_0",
                                                          "NORMAL",
                                                          "TEXCOORD_0",
                                                          "TANGENT" };

// TODO: needs big refactor, break into smaller functions PLEASE, add animation support
Ref<geometry::Model> GLTFImporter::importGLTF(const fs::path& path)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool success;

    if (path.extension() == ".gltf") {
        success = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path.string());
    } else {
        success = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path.string());
    }

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

    SirenAssert(gltfModel.scenes.size() == 1, "Only gltf files with one scene are suppoerted");

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
            if (bct.index != -1) { material->baseColorMap = textures[bct.index]; }
        }

        // Metallic Roughness
        {
            const tinygltf::TextureInfo& mrt =
                gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture;

            material->metallicFactor  = gltfMaterial.pbrMetallicRoughness.metallicFactor;
            material->roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
            if (mrt.index != -1) { material->metallicRoughnessMap = textures[mrt.index]; }
        }

        // Emission
        {
            const std::vector<double>& ef   = gltfMaterial.emissiveFactor;
            const tinygltf::TextureInfo& et = gltfMaterial.emissiveTexture;

            material->emissionColor = glm::vec3(ef[1], ef[1], ef[2]);
            if (et.index != -1) { material->emissionMap = textures[et.index]; }
        }

        // Occlusion
        {
            const tinygltf::OcclusionTextureInfo& ot = gltfMaterial.occlusionTexture;

            material->occlusionStrength = ot.strength;
            if (ot.index != -1) { material->occlusionMap = textures[ot.index]; }
        }

        // Normals
        {
            const tinygltf::NormalTextureInfo& nt = gltfMaterial.normalTexture;

            material->normalScale = nt.scale;
            if (nt.index != -1) { material->normalMap = textures[nt.index]; }
        }

        materials.push_back(material);
    }

    // perform dfs on scene nodes for transforms
    std::unordered_map<int, glm::mat4> meshTransforms{};
    std::stack<std::pair<int, glm::mat4>> nodes; // node index + parent transform
    for (int n : gltfModel.scenes[0].nodes) { nodes.push({ n, glm::mat4(1.0f) }); }

    while (!nodes.empty()) {
        auto [nodeIndex, parentTransform] = nodes.top();
        nodes.pop();
        const tinygltf::Node& node = gltfModel.nodes[nodeIndex];

        // compute local transform
        glm::mat4 local = glm::mat4(1.0f);
        if (node.matrix.size() == 16) {
            local = glm::make_mat4(node.matrix.data());
        } else {
            glm::vec3 translation = node.translation.size()
                                        ? glm::vec3(glm::make_vec3(node.translation.data()))
                                        : glm::vec3(0.0f);
            glm::quat rotation    = node.rotation.size()
                                        ? glm::quat(glm::vec4(glm::make_vec4(node.rotation.data())))
                                        : glm::quat(1.0f, 0, 0, 0);
            glm::vec3 scale =
                node.scale.size() ? glm::vec3(glm::make_vec3(node.scale.data())) : glm::vec3(1.0f);
            local = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) *
                    glm::scale(glm::mat4(1.0f), scale);
        }

        glm::mat4 global = parentTransform * local; // accumulate parent transform

        if (node.mesh != -1) { meshTransforms[node.mesh] = global; }

        for (int child : node.children) { nodes.push({ child, global }); }
    }

    for (size_t mIndex = 0; mIndex < gltfModel.meshes.size(); mIndex++) {
        const auto& m = gltfModel.meshes[mIndex];
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
                // TODO: this looks suspicious
                copyToBufferWithStride(indexData,
                                       buffer.data,
                                       bufferView.byteOffset + accessor.byteOffset,
                                       bufferView.byteOffset + accessor.byteOffset +
                                           accessor.count * indexType.size(),
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

            // TODO: hardcoded for now, since we enforce this buffer layout in the uber shader
            bufferLayout.addVertexAttribute({ "POSITION", 3, GL_FLOAT });
            bufferLayout.addVertexAttribute({ "COLOR_0", 4, GL_FLOAT });
            bufferLayout.addVertexAttribute({ "NORMAL", 3, GL_FLOAT });
            bufferLayout.addVertexAttribute({ "TEXCOORD_0", 2, GL_FLOAT });
            bufferLayout.addVertexAttribute({ "TANGENT", 4, GL_FLOAT });
            bufferLayout.close();
            offsets = { 0, 0, 0, 0, 0 };

            for (int i = 0; i < vertexCount; i++) {
                for (int j = 0; j < allowedGltfAttributes.size(); j++) {
                    const std::string& name = allowedGltfAttributes[j];

                    // only use certain attributes so fill in dummy data
                    if (!prim.attributes.contains(allowedGltfAttributes[j])) {
                        std::vector<float> dummy;
                        if (name == "COLOR_0") {
                            dummy = { 0, 0, 0, 1 }; // always vec4
                        } else if (name == "NORMAL") {
                            dummy = { 0, 0, 1 };
                        } else if (name == "TEXCOORD_0") {
                            dummy = { 0, 0 };
                        } else if (name == "TANGENT") {
                            dummy = { 0, 0, 0, 0 };
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
                prim.material != -1 ? materials[prim.material] : nullptr;

            const glm::mat4 transform =
                meshTransforms.contains(mIndex) ? meshTransforms.at(mIndex) : glm::mat4{ 1 };
            model->addMesh({ mat, vao, transform });
        }
    }

    return model;
}
} // namespace siren::assets
