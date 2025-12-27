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
    auto& am = assets();
    auto& rd = renderer();

    // find the active camera to render from
    RenderContextComponent* rcc = scene.getSingletonSafe<RenderContextComponent>();
    if (!rcc->cameraComponent) { return; } // cannot draw

    CameraInfo cameraInfo{
        rcc->cameraComponent->getProjMat(),
        rcc->cameraComponent->getViewMat(),
        rcc->cameraComponent->position
    };
    // todo: setup these structs!
    LightInfo lightInfo{ };
    EnvironmentInfo envInfo{ };
    RenderInfo renderInfo{ };

    // setup lights
    {
        i32 lightCount = 0;
        for (const auto& lightEntity : scene.getWith<core::PointLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 PointLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& pointLightComponent = scene.getSafe<core::PointLightComponent>(lightEntity);
            if (!pointLightComponent) { continue; }
            lightInfo.pointLights[lightCount] = core::GPUPointLight(
                pointLightComponent->position,
                pointLightComponent->color
            );
            lightCount++;
        }
        lightInfo.pointLightCount = lightCount;
        lightCount                = 0;
        for (const auto& lightEntity : scene.getWith<core::DirectionalLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 DirectionalLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& directionalLightComponent = scene.getSafe<core::DirectionalLightComponent>(lightEntity);
            if (!directionalLightComponent) { continue; }
            lightInfo.directionalLights[lightCount] = core::GPUDirectionalLight(
                directionalLightComponent->direction,
                directionalLightComponent->color
            );
            lightCount++;
        }
        lightInfo.directionalLightCount = lightCount;
        lightCount                      = 0;
        for (const auto& lightEntity : scene.getWith<core::SpotLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 SpotLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& spotLightComponent = scene.getSafe<core::SpotLightComponent>(lightEntity);
            if (!spotLightComponent) { continue; }
            lightInfo.spotLights[lightCount] = core::GPUSpotLight(
                spotLightComponent->position,
                spotLightComponent->color,
                spotLightComponent->innerCone,
                spotLightComponent->outerCone
            );
            lightCount++;
        }
        lightInfo.spotLightCount = lightCount;
        lightCount               = 0;
    }

    // setup environment
    {
        const auto rcc = scene.getSingletonSafe<core::RenderContextComponent>();
        if (rcc && rcc->skyBoxComponent) {
            const auto cubeMap = am.getAsset<core::TextureCubeMap>(rcc->skyBoxComponent->cubeMapHandle);
            if (cubeMap) {
                envInfo.skybox = cubeMap;
            } else {
                Todo;
                // envInfo.skybox = am.getFallback<core::TextureCubeMap>();
            }
        }
    }

    rd.beginFrame({ cameraInfo, lightInfo, envInfo });
    rd.beginPass(nullptr, glm::vec4{ 0.14, 0.14, 0.14, 1 });

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

    rd.endPass();
    rd.endFrame();
}
} // namespace siren::ecs
