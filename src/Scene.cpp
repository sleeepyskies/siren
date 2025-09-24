#include "Scene.hpp"

Scene::Scene(const sobj::OBJData& data, BufferUsage usage, const int width, const int height)
    : m_camera(width, height)
{
    // first load all Images into textures
    std::vector<ref<Texture2D>> textures{};
    for (const auto& [name, bytes, width, height, channels] : data.images) {
        const ref<Texture2D> texture = makeRef<Texture2D>(name, bytes, width, height, channels);
        textures.push_back(texture);
    }

    // create materials using the images
    std::vector<ref<Material>> materials{};
    for (const auto& sobjMat : data.materials) {
        const ref<Material> material = makeRef<Material>(sobjMat.name);

        if (sobjMat.ambientMapIndex) {
            material->addAmbientMap(textures[*sobjMat.ambientMapIndex]);
        }
        if (sobjMat.diffuseMapIndex) {
            material->addDiffuseMap(textures[*sobjMat.diffuseMapIndex]);
        }
        if (sobjMat.specularMapIndex) {
            material->addSpecularMap(textures[*sobjMat.specularMapIndex]);
        }
        if (sobjMat.roughnessMapIndex) {
            material->addRoughnessMap(textures[*sobjMat.roughnessMapIndex]);
        }
        if (sobjMat.alphaMapIndex) {
            material->addAlphaMap(textures[*sobjMat.alphaMapIndex]); // pad
        }

        if (sobjMat.ambient) {
            material->addAmbience(
                glm::vec3(sobjMat.ambient->x, sobjMat.ambient->y, sobjMat.ambient->z));
        }
        if (sobjMat.diffuse) {
            material->addDiffuse(
                glm::vec3(sobjMat.diffuse->x, sobjMat.diffuse->y, sobjMat.diffuse->z));
        }
        if (sobjMat.specular) {
            material->addSpecular(
                glm::vec3(sobjMat.specular->x, sobjMat.specular->y, sobjMat.specular->z));
        }
        if (sobjMat.roughness) {
            material->addRoughness(*sobjMat.roughness); // padding
        }
        if (sobjMat.alpha) {
            material->addAlpha(*sobjMat.alpha); // padding
        }

        materials.push_back(material);
    }

    const auto glmVec3 = [](const sobj::Vec3& vec) { return glm::vec3{ vec.x, vec.y, vec.z }; };
    const auto glmVec2 = [](const sobj::Vec2& vec) { return glm::vec2{ vec.x, vec.y }; };

    std::vector<Vertex> vertices{};
    uint32_t current = 0;
    std::vector<uint32_t> indices{}; // currently always sequential since we store redundant
                                     // vertices, this is a fault of sobj though
    for (const auto& objMesh : data.meshes) {
        std::vector<uint32_t> meshIndices{};
        for (const auto& face : objMesh.faces) {
            // construct the vertex, this is painful >:(
            // we have lots of redundant data here
            // face must always have same type of vertices
            for (int i = 0; i < face.numVertices(); ++i) {
                Vertex v;
                // always need position
                v.position = glmVec3(data.positions[face.positionIndices[i]]);
                if (!face.normalIndices.empty())
                    v.normal = glmVec3(data.normals[face.normalIndices[i]]);
                if (!face.uvIndices.empty())
                    v.uv = glmVec2(data.textureUVs[face.uvIndices[i]]); // padding
                if (!face.colorIndices.empty())
                    v.color = glmVec3(data.colors[face.colorIndices[i]]);

                vertices.push_back(v);
                meshIndices.push_back(current++);
            }
        }
        m_meshes.push_back(makeUref<Mesh>(meshIndices, objMesh.name));
        if (objMesh.materialIndex) // material setting
            m_meshes.back()->setMaterial(materials[*objMesh.materialIndex]);
    }

    // setup this class finally
    m_vertices = vertices;
    m_name     = data.name;
    m_vbo.uploadData(m_vertices, usage);

    for (const auto& m : m_meshes) {
        m->init(m_vbo, usage);
    }
}

void Scene::onUpdate(const float delta, const Window& window)
{
    m_camera.onUpdate(delta, window);
}

void Scene::onWindowResize(const int width, const int height)
{
    m_camera.setViewportWidth(width);
    m_camera.setViewportHeight(height);
}

void Scene::draw(const Shaders& shaders) const
{
    shaders.use();

    shaders.setUniformMat4("uProjView", m_camera.projectionViewMatrix());
    shaders.setUniformVec3("uCameraPos", m_camera.position());

    // FIXME: hardcoded
    shaders.setUniformVec3("uLightPos", { 2, 2, 2 });
    shaders.setUniformVec3("uLightColor", { 1, 1, 1 });

    for (const auto& mesh : m_meshes) {
        shaders.setUniformMat4("uModel", m_globalTransform * mesh->modelMatrix());
        mesh->bind(shaders);
        mesh->draw();
    }
}

void Scene::translate(const glm::vec3 translation)
{
    m_globalTransform = glm::translate(m_globalTransform, translation);
}

void Scene::scale(const glm::vec3 scale)
{
    m_globalTransform = glm::scale(m_globalTransform, scale);
}

void Scene::scale(const float scalar)
{
    scale(glm::vec3(scalar));
}

void Scene::rotateDeg(const glm::vec3 axis, const float degrees)
{
    m_globalTransform = glm::rotate(m_globalTransform, glm::radians(degrees), axis);
}

void Scene::rotateRad(const glm::vec3 axis, const float radians)
{
    m_globalTransform = glm::rotate(m_globalTransform, radians, axis);
}

glm::mat4 Scene::getTransform() const
{
    return m_globalTransform;
}
