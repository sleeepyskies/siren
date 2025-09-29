#include "Scene.hpp"

#include <random>

namespace core
{

Scene::Scene(const Window& window) : m_camera(window.getSize().x, window.getSize().y)
{
    sobj::OBJLoader loader{};
    loader.load("../resources/models/obj/box/untitled.obj");
    sobj::OBJData objData = loader.steal();

    // random boxes
    constexpr int amount = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-20.0f, 20.0f);  // translation range
    std::uniform_real_distribution<float> angleDist(0.0f, 360.0f); // rotation in degrees
    std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);   // rotation axis components
    std::uniform_real_distribution<float> colorDist(0.8f, 1.0f);   // color random

    for (int i = 0; i < amount; ++i) {
        auto model = makeUref<Model>(objData, renderer::STATIC);

        // Random translation
        model->translate(glm::vec3(posDist(gen), posDist(gen), posDist(gen)));

        // Random rotation
        glm::vec3 axis(axisDist(gen), axisDist(gen), axisDist(gen));
        if (glm::length(axis) > 0.0f) axis = glm::normalize(axis);
        model->rotateDeg(axis, angleDist(gen));

        m_models.push_back(std::move(model));
    }

    // lights, not great solution lol
    constexpr int amountLights = 1;

    loader.load("../resources/models/obj/cube.obj");
    sobj::OBJData cube = loader.steal();

    // Initialize point lights with random positions/colors
    for (int i = 0; i < amountLights; ++i) {
        glm::vec3 pos(posDist(gen), posDist(gen), posDist(gen));
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        // glm::vec3 color(1);
        m_pointLights.emplace_back(color, pos);

        auto model = makeUref<Model>(cube, renderer::STATIC);

        // Align model with light
        model->translate(pos);
        m_lightModels.push_back(std::move(model));
    }

    auto dirData         = reinterpret_cast<const unsigned char*>(m_directionalLights.data());
    const size_t dirSize = m_directionalLights.size() * sizeof(DirectionalLight);

    auto pntData         = reinterpret_cast<const unsigned char*>(m_pointLights.data());
    const size_t pntSize = m_pointLights.size() * sizeof(PointLight);

    auto sptData         = reinterpret_cast<const unsigned char*>(m_spotLights.data());
    const size_t sptSize = m_spotLights.size() * sizeof(SpotLight);

    uint8_t slot = 0;
    m_dirLightBuffer.uploadData(std::vector(dirData, dirData + dirSize), renderer::STATIC, slot++);
    m_pointLightBuffer.uploadData(
        std::vector(pntData, pntData + pntSize), renderer::STATIC, slot++);
    m_spotLightBuffer.uploadData(std::vector(sptData, sptData + sptSize), renderer::STATIC, slot++);
}

void Scene::onUpdate(const float delta, const Window& window)
{
    m_camera.onUpdate(delta, window);
    for (auto& m : m_models) {
        // m.onUpdate(delta, window);
    }
    // lights??
}

void Scene::draw(const renderer::Shader& objectShader, const renderer::Shader& lightShader) const
{
    objectShader.use();

    objectShader.setUniformMat4("uProjView", m_camera.projectionViewMatrix());
    objectShader.setUniformVec3("uCameraPos", m_camera.position());

    for (const auto& m : m_models) {
        m->draw(objectShader);
    }

    // render light meshes, this is a bad quickfix tho
    lightShader.use();
    lightShader.setUniformMat4("uProjView", m_camera.projectionViewMatrix());
    lightShader.setUniformVec3("uCameraPos", m_camera.position());

    for (size_t i = 0; i < m_lightModels.size(); ++i) {
        const auto& m = m_lightModels[i];
        lightShader.setUniformInt("uLightIndex", i);
        lightShader.setUniformInt("uLightType", 1); // e.g., 2 == point light
        m->draw(lightShader);
    }
}

void Scene::onWindowResize(const int width, const int height)
{
    m_camera.setViewportWidth(width);
    m_camera.setViewportHeight(height);
}

} // namespace core
