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
    auto& am       = core::assets();
    auto& renderer = core::renderer();

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
            lightInfo.pointLights[lightCount] = core::GPUPointLight(
                pointLightComponent->position,
                pointLightComponent->color
            );
            lightCount++;
        }
        lightInfo.pointLightCount = lightCount;
        lightCount                = 0;
        for (const auto& lightEntity : scene->getWith<core::DirectionalLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 DirectionalLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& directionalLightComponent = scene->getSafe<core::DirectionalLightComponent>(lightEntity);
            if (!directionalLightComponent) { continue; }
            lightInfo.directionalLights[lightCount] = core::GPUDirectionalLight(
                directionalLightComponent->direction,
                directionalLightComponent->color
            );
            lightCount++;
        }
        lightInfo.directionalLightCount = lightCount;
        lightCount                      = 0;
        for (const auto& lightEntity : scene->getWith<core::SpotLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 SpotLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& spotLightComponent = scene->getSafe<core::SpotLightComponent>(lightEntity);
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

    renderer.beginFrame({ cameraInfo, lightInfo });
    renderer.beginPass(frameBuffer, glm::vec4{ 0.1, 0.4, 0.2, 1 });

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
            renderer.submit(vertexArray, material, meshTransform);
        }
    }

    renderer.endFrame();

    // log stats
    return;
    {
        const auto& [drawCalls, triangles, vertices, textureBinds, shaderBinds] = renderer.getStats();
        dbg("Render Stats:");
        dbg("   Draw Calls: {}", drawCalls);
        dbg("   Triangles: {}", triangles);
        dbg("   Vertices: {}", vertices);
        dbg("   Texture Binds: {}", textureBinds);
        dbg("   Shader Binds: {}", shaderBinds);
    }
}

} // namespace siren::editor
