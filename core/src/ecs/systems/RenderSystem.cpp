#include "RenderSystem.hpp"

#include "core/Application.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "geometry/Material.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"

namespace siren::ecs
{

void RenderSystem::onRender(ecs::Scene& scene)
{
    const auto& am = core::Application::get().getAssetManager();

    // find the active camera to render from
    RenderContextComponent* rcc = scene.getSingletonSafe<RenderContextComponent>();
    if (!rcc->cameraComponent || !rcc->basicObjectShader) { return; } // cannot draw

    // HACK: do not hardcode shader
    const auto shader = am.getAsset<renderer::Shader>(rcc->basicObjectShader);

    renderer::CameraProperties cameraProperties{ rcc->cameraComponent->getProjViewMat(),
                                                 rcc->cameraComponent->position };
    renderer::Renderer::beginScene(cameraProperties);

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
            renderer::Renderer::draw(
                mesh->getVertexArray(), mesh->getMaterial(), meshTransform, shader);
        }
    }

    renderer::Renderer::endScene();
}

} // namespace siren::ecs
