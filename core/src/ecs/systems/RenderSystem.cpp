#include "RenderSystem.hpp"

#include "assets/AssetModule.hpp"
#include "core/Core.hpp"
#include "renderer/RenderModule.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/renderConfig.hpp"

namespace siren::core
{

void RenderSystem::onRender(Scene& scene)
{
    NotImplemented; /// fixme

    auto& am = assets();
    auto& rd = renderer();

    // find the active camera to render from
    RenderContextComponent* rcc = scene.getSingletonSafe<RenderContextComponent>();
    if (!rcc->cameraComponent) { return; } // cannot draw

    CameraInfo cameraInfo{
        rcc->cameraComponent->getProjViewMat(),
        rcc->cameraComponent->position
    };
    // todo: setup these structs!
    LightInfo lightInfo{};
    EnvironmentInfo environmentInfo{};
    RenderInfo renderInfo{};
    rd.beginFrame(renderInfo);

    // iterate over all drawable entities
    for (const auto& e : scene.getWith<MeshComponent, TransformComponent>()) {
        const auto* meshComponent      = scene.getSafe<MeshComponent>(e);
        const auto* transformComponent = scene.getSafe<TransformComponent>(e);

        if (!meshComponent || !transformComponent) { continue; } // not enough info to draw

        const auto mesh          = am.getAsset<Mesh>(meshComponent->meshHandle);
        const auto meshTransform = transformComponent->getTransform();

        for (const auto& [localTransform, materialHandle, vertexArray] : mesh->getSurfaces()) {
            const glm::mat4 surfaceTransform = meshTransform * localTransform;
            Ref<Material> material           = am.getAsset<Material>(materialHandle);
            rd.submit(vertexArray, material, surfaceTransform);
        }
    }

    rd.endFrame();
}

} // namespace siren::ecs