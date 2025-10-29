#include "SceneViewRenderer.hpp"

#include "EditorApp.hpp"
#include "EditorCamera.hpp"
#include "assets/AssetModule.hpp"
#include "ecs/Components.hpp"
#include "geometry/Mesh.hpp"
#include "renderer/FrameBuffer.hpp"
#include "renderer/RenderModule.hpp"


namespace siren::editor
{

SceneViewRenderer::SceneViewRenderer()
{
    auto& am               = core::assets();
    const auto planeHandle = am.createPrimitive(
        core::PlaneParams{
            .width = 50,
            .depth = 50,
            .widthSegments = 1,
            .depthSegments = 1
        }
    );
    if (planeHandle) {
        m_plane = am.getAsset<core::Mesh>(*planeHandle);
    }
}

void SceneViewRenderer::render(
    const Ref<core::Scene>& scene,
    const Ref<EditorCamera>& camera,
    const Ref<core::FrameBuffer>& frameBuffer
) const
{
    auto& am = core::assets();
    auto& rd = core::renderer();

    rd.setFrameBuffer(frameBuffer);
    frameBuffer->bind();

    // maybe make a EditorSceneRenderer class?
    const core::CameraInfo cameraInfo{ camera->getProjViewMat(), camera->getPosition() };
    core::LightInfo lightInfo{ };

    // setup lights
    {
        i32 lightCount = 0;
        for (const auto& lightEntity : scene->getWith<core::PointLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 PointLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& pointLightComponent = scene->getSafe<core::PointLightComponent>(lightEntity);
            if (!pointLightComponent) { continue; }
            lightCount++;
            lightInfo.pointLights[lightCount] = core::GPUPointLight(
                pointLightComponent->position,
                pointLightComponent->color
            );
            lightInfo.pointLightCount = lightCount;
        }
        lightCount = 0;
        for (const auto& lightEntity : scene->getWith<core::DirectionalLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 DirectionalLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& directionalLightComponent = scene->getSafe<core::DirectionalLightComponent>(lightEntity);
            if (!directionalLightComponent) { continue; }
            lightCount++;
            lightInfo.directionalLights[lightCount] = core::GPUDirectionalLight(
                directionalLightComponent->direction,
                directionalLightComponent->color
            );
            lightInfo.directionalLightCount = lightCount;
        }
        lightCount = 0;
        for (const auto& lightEntity : scene->getWith<core::SpotLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 SpotLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& spotLightComponent = scene->getSafe<core::SpotLightComponent>(lightEntity);
            if (!spotLightComponent) { continue; }
            lightCount++;
            lightInfo.spotLights[lightCount] = core::GPUSpotLight(
                spotLightComponent->position,
                spotLightComponent->color,
                spotLightComponent->innerCone,
                spotLightComponent->outerCone
            );
            lightInfo.spotLightCount = lightCount;
        }
    }

    rd.begin({ cameraInfo, lightInfo });

    // iterate over all drawable entities
    for (const auto& e : scene->getWith<core::MeshComponent, core::TransformComponent>()) {
        const auto* modelComponent     = scene->getSafe<core::MeshComponent>(e);
        const auto* transformComponent = scene->getSafe<core::TransformComponent>(e);

        if (!modelComponent || !transformComponent) { continue; } // not enough info to draw

        const auto model     = am.getAsset<core::Mesh>(modelComponent->meshHandle);
        const auto transform = transformComponent->getTransform();

        for (const auto& [surfaceTransform, materialHandle, vertexArray] : model->getSurfaces()) {
            const glm::mat4 meshTransform = transform * surfaceTransform;
            const auto& material          = am.getAsset<core::Material>(materialHandle);
            rd.submit(vertexArray, material, meshTransform);
        }
    }

    rd.end();
    frameBuffer->unbind();
}

} // namespace siren::editor
