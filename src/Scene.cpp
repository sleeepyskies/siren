#include "Scene.hpp"

Scene::Scene(const Window& window) : m_camera(window.getWidth(), window.getHeight())
{
    sobj::OBJLoader loader{};
    loader.load("../resources/models/obj/car/untitled.obj");
    sobj::OBJData objData = loader.steal();

    m_models.push_back(makeUref<Model>(objData, STATIC, window.getWidth(), window.getHeight()));
    m_models[0]->scale(0.05);
}

void Scene::onUpdate(const float delta, const Window& window)
{
    m_camera.onUpdate(delta, window);
    for (auto& m : m_models) {
        // m.onUpdate(delta, window);
    }
    for (auto& l : m_lights) {
        // pass for now
    }
}

void Scene::draw(const Shaders& shaders) const
{
    shaders.use();

    shaders.setUniformMat4("uProjView", m_camera.projectionViewMatrix());
    shaders.setUniformVec3("uCameraPos", m_camera.position());

    // FIXME: hardcoded, sent light data buffer?
    shaders.setUniformVec3("uLightPos", { 156, 500, 5 });
    shaders.setUniformVec3("uLightColor", glm::vec3{ 0 });

    for (const auto& m : m_models) {
        m->draw(shaders);
    }
}

void Scene::onWindowResize(const int width, const int height)
{
    m_camera.setViewportWidth(width);
    m_camera.setViewportHeight(height);
}
