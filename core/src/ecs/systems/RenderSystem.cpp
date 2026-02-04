#include "RenderSystem.hpp"

#include "core/Core.hpp"
#include "renderer/RenderModule.hpp"
#include "ecs/Components.hpp"
#include "ecs/core/World.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/RenderInfo.hpp"


namespace siren::core
{
void RenderSystem::onRender(World& scene) {
    auto& am = Assets();
    auto& rd = Renderer();

    // find the active camera to render from
    const RenderContextComponent* rcc = scene.GetSingletonSafe<RenderContextComponent>();
    if (!rcc->cameraComponent) { return; } // cannot draw

    const CameraInfo cameraInfo{
        rcc->cameraComponent->getProjMat(),
        rcc->cameraComponent->getViewMat(),
        rcc->cameraComponent->position
    };

    LightInfo lightInfo;
    EnvironmentInfo envInfo;

    // setup lights
    {
        i32 lightCount = 0;
        for (const auto& lightEntity : scene.GetWith<PointLightComponent, TransformComponent>()) {
            if (lightCount >= MAX_LIGHT_COUNT) {
                wrn(
                    "There are more than MAX_LIGHT_COUNT = {} PointLight's in the current scene, cannot render them all.",
                    MAX_LIGHT_COUNT
                );
                break;
            }
            const auto& pointLightComponent = scene.GetSafe<PointLightComponent>(lightEntity);
            const auto& transformComponent  = scene.GetSafe<TransformComponent>(lightEntity);
            if (!pointLightComponent) { continue; }
            lightInfo.pointLights[lightCount] = GPUPointLight(
                transformComponent->translation,
                pointLightComponent->color
            );
            lightCount++;
        }
        lightInfo.pointLightCount = lightCount;
        lightCount                = 0;
        for (const auto& lightEntity : scene.GetWith<DirectionalLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 DirectionalLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& directionalLightComponent = scene.GetSafe<DirectionalLightComponent>(lightEntity);
            if (!directionalLightComponent) { continue; }
            lightInfo.directionalLights[lightCount] = GPUDirectionalLight(
                directionalLightComponent->direction,
                directionalLightComponent->color
            );
            lightCount++;
        }
        lightInfo.directionalLightCount = lightCount;
        lightCount                      = 0;
        for (const auto& lightEntity : scene.GetWith<SpotLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 SpotLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& spotLightComponent = scene.GetSafe<SpotLightComponent>(lightEntity);
            if (!spotLightComponent) { continue; }
            lightInfo.spotLights[lightCount] = GPUSpotLight(
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
        if (rcc->skyBoxComponent) {
            const auto cubeMap = am.GetAsset<TextureCubeMap>(rcc->skyBoxComponent->cubeMapHandle);
            if (cubeMap) {
                envInfo.skybox = cubeMap;
            } else {
                TODO;
                // envInfo.skybox = am.getFallback<core::TextureCubeMap>();
            }
        }
    }

    rd.BeginFrame({ cameraInfo, lightInfo, envInfo });
    rd.BeginPass(nullptr, glm::vec4{ 0.14, 0.14, 0.14, 1 });

    // iterate over all drawable entities
    for (const auto& e : scene.GetWith<MeshComponent, TransformComponent>()) {
        const auto* meshComponent      = scene.GetSafe<MeshComponent>(e);
        const auto* transformComponent = scene.GetSafe<TransformComponent>(e);

        if (!meshComponent || !transformComponent) { continue; } // not enough info to draw

        const auto mesh          = am.GetAsset<Mesh>(meshComponent->meshHandle);
        const auto meshTransform = transformComponent->GetTransform();

        Renderer().SubmitMesh(mesh, meshTransform);
    }

    rd.EndPass();
    rd.EndFrame();
}
} // namespace siren::ecs
