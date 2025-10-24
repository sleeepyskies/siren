#include "SceneViewRenderer.hpp"

#include "EditorApp.hpp"
#include "EditorCamera.hpp"
#include "ecs/Components.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/FrameBuffer.hpp"
#include "renderer/Renderer.hpp"

namespace siren::editor
{

SceneViewRenderer::SceneViewRenderer()
{
    auto& am               = EditorApp::get().getAssetManager();
    const auto planeHandle = am.createPrimitive<geometry::PlaneMesh>();
    m_plane                = am.getAsset<geometry::PlaneMesh>(planeHandle);

    m_plane->setParams({ .width = 50, .height = 50, .subdivisionsX = 1, .subdivisionsY = 1 });
}

void SceneViewRenderer::render(const Ref<ecs::Scene>& scene, const Ref<EditorCamera>& camera,
                               const Ref<renderer::FrameBuffer>& frameBuffer) const
{
    frameBuffer->bind();
    frameBuffer->prepare();
    const auto& am = EditorApp::get().getAssetManager();

    // maybe make a EditorSceneRenderer class?
    const renderer::CameraInfo cameraInfo{ camera->getProjViewMat(), camera->getPosition() };
    renderer::LightInfo lightInfo{};
    renderer::EnvironmentInfo environmentInfo{};

    // setup lights
    for (const auto& lightEntity : scene->getWith<ecs::PointLightComponent>()) {
        const auto& pointLightComponent = scene->getSafe<ecs::PointLightComponent>(lightEntity);
        if (!pointLightComponent) { continue; }

        lightInfo.pointLights.emplace_back(pointLightComponent->position,
                                           pointLightComponent->color);
    }

    renderer::Renderer::begin({ cameraInfo, lightInfo, environmentInfo });

    // iterate over all drawable entities
    for (const auto& e : scene->getWith<ecs::ModelComponent, ecs::TransformComponent>()) {
        const auto* modelComponent     = scene->getSafe<ecs::ModelComponent>(e);
        const auto* transformComponent = scene->getSafe<ecs::TransformComponent>(e);

        if (!modelComponent || !transformComponent) { continue; } // not enough info to draw

        const auto model     = am.getAsset<geometry::Model>(modelComponent->modelHandle);
        const auto transform = transformComponent->getTransform();

        for (const auto& meshHandle : model->getMeshHandles()) {
            const auto mesh               = am.getAsset<geometry::Mesh>(meshHandle);
            const glm::mat4 meshTransform = transform * mesh->getLocalTransform();
            const auto& material = am.getAsset<renderer::Material>(mesh->getMaterialHandle());
            renderer::Renderer::draw(mesh->getVertexArray(), material, meshTransform);
        }
    }

    renderer::Renderer::end();
    frameBuffer->unbind();
}

} // namespace siren::editor