#include "RenderSystem.hpp"

#include "core/Application.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/material/Material.hpp"

namespace siren::ecs
{

void RenderSystem::onRender(Scene& scene)
{
    NotImplemented;

    const auto& am = core::Application::get().getAssetManager();

    // find the active camera to render from
    RenderContextComponent* rcc = scene.getSingletonSafe<RenderContextComponent>();
    if (!rcc->cameraComponent) { return; } // cannot draw

    renderer::CameraInfo cameraInfo{ rcc->cameraComponent->getProjViewMat(),
                                     rcc->cameraComponent->position };
    renderer::LightInfo lightInfo{};
    renderer::EnvironmentInfo environmentInfo{};
    renderer::RenderInfo renderInfo{};
    renderer::Renderer::begin(renderInfo);

    // iterate over all drawable entities
    for (const auto& e : scene.getWith<ModelComponent, TransformComponent>()) {
        const auto* modelComponent     = scene.getSafe<ModelComponent>(e);
        const auto* transformComponent = scene.getSafe<TransformComponent>(e);

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
}

} // namespace siren::ecs
