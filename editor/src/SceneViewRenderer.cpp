#include "SceneViewRenderer.hpp"

#include "EditorApp.hpp"
#include "EditorCamera.hpp"
#include "assets/AssetModule.hpp"
#include "ecs/Components.hpp"

#include "../../core/src/core/file_system.hpp"

#include "geometry/mesh.hpp"
#include "../../core/src/renderer/resources/framebuffer.hpp"
#include "renderer/renderer.hpp"


namespace siren::editor
{

SceneViewRenderer::SceneViewRenderer()
{
    createEditorGrid();
}

void SceneViewRenderer::render(
    const core::World& scene,
    const Ref<EditorCamera>& camera,
    const Ref<core::Framebuffer>& frameBuffer
) const
{
    auto& am       = core::Assets();
    auto& renderer = core::Renderer();

    const core::CameraInfo cameraInfo{
        camera->getProjMat(),
        camera->getViewMat(),
        camera->getPosition()
    };
    core::LightInfo lightInfo{ };
    core::EnvironmentInfo envInfo{ };

    // setup lights
    {
        i32 lightCount = 0;
        for (const auto& lightEntity : scene.GetWith<core::PointLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 PointLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& pointLightComponent = scene.GetSafe<core::PointLightComponent>(lightEntity);
            if (!pointLightComponent) { continue; }
            lightInfo.pointLights[lightCount] = core::GPUPointLight(
                pointLightComponent->position,
                pointLightComponent->color
            );
            lightCount++;
        }
        lightInfo.pointLightCount = lightCount;
        lightCount                = 0;
        for (const auto& lightEntity : scene.GetWith<core::DirectionalLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 DirectionalLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& directionalLightComponent = scene.GetSafe<core::DirectionalLightComponent>(lightEntity);
            if (!directionalLightComponent) { continue; }
            lightInfo.directionalLights[lightCount] = core::GPUDirectionalLight(
                directionalLightComponent->direction,
                directionalLightComponent->color
            );
            lightCount++;
        }
        lightInfo.directionalLightCount = lightCount;
        lightCount                      = 0;
        for (const auto& lightEntity : scene.GetWith<core::SpotLightComponent>()) {
            if (lightCount >= 16) {
                wrn("There are more than 16 SpotLight's in the current scene, cannot render them all.");
                break;
            }
            const auto& spotLightComponent = scene.GetSafe<core::SpotLightComponent>(lightEntity);
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
        const auto rcc = scene.GetSingletonSafe<core::RenderContextComponent>();
        if (rcc && rcc->skyBoxComponent) {
            const auto cubeMap = am.GetAsset<core::TextureCubeMap>(rcc->skyBoxComponent->cubeMapHandle);
            if (cubeMap) {
                envInfo.skybox = cubeMap;
            } else {
                TODO;
                // envInfo.skybox = am.getFallback<core::TextureCubeMap>();
            }
        }
    }

    renderer.begin_frame({ cameraInfo, lightInfo, envInfo });
    renderer.begin_pass(frameBuffer, glm::vec4{ 0.14, 0.14, 0.14, 1 });

    // iterate over all drawable entities
    for (const auto& e : scene.GetWith<core::MeshComponent, core::TransformComponent>()) {
        const auto* meshComponent      = scene.GetSafe<core::MeshComponent>(e);
        const auto* transformComponent = scene.GetSafe<core::TransformComponent>(e);

        if (!meshComponent || !meshComponent->meshHandle || !transformComponent) { continue; }

        const auto mesh      = am.GetAsset<core::Mesh>(meshComponent->meshHandle);
        const auto transform = transformComponent->GetTransform();

        for (const auto& [surfaceTransform, materialHandle, vertexArray] : mesh->GetSurfaces()) {
            const glm::mat4 meshTransform = transform * surfaceTransform;
            const auto& material          = am.GetAsset<core::PBRMaterial>(materialHandle);
            renderer.submit(vertexArray, material, meshTransform);
        }
    }

    // editor grid lines
    if (m_editorGrid.enabled) {
        glm::mat4 modelTransform{ 1 };
        modelTransform = translate(modelTransform, glm::vec3(camera->getPosition().x, 0, camera->getPosition().z));
        renderer.submit(m_editorGrid.mesh, m_editorGrid.material, m_editorGrid.shader, modelTransform);
    }

    renderer.end_frame();
}

void SceneViewRenderer::createEditorGrid()
{
    m_editorGrid.material              = create_ref<core::PBRMaterial>("Editor Grid Material");
    m_editorGrid.material->doubleSided = true;
    m_editorGrid.material->baseColor   = glm::vec4(0, 0, 0, 0);
    m_editorGrid.material->alphaMode   = core::PBRMaterial::AlphaMode::BLEND;

    m_editorGrid.mesh = core::primitive::generatePlane(
        { .width = 1000, .depth = 1000, .widthSegments = 1, .depthSegments = 1 }
    );
    m_editorGrid.transform = glm::mat4{ 1 };
    m_editorGrid.enabled   = true;

    const auto& fs                = core::filesystem();
    const auto& shaderPath        = fs.getEngineRoot() / "assets" / "shaders";
    const std::string& vertSource = fs.readFile(shaderPath / "basic.vert");
    const std::string& fragSource = fs.readFile(shaderPath / "grid.frag");

    m_editorGrid.shader = create_ref<core::Shader>("Editor Grid Shader", vertSource, fragSource);
}

} // namespace siren::editor
