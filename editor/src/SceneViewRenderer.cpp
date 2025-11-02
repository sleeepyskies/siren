#include "SceneViewRenderer.hpp"

#include "EditorApp.hpp"
#include "EditorCamera.hpp"
#include "assets/AssetModule.hpp"
#include "ecs/Components.hpp"

#include "filesystem/FileSystemModule.hpp"

#include "geometry/Mesh.hpp"
#include "renderer/FrameBuffer.hpp"
#include "renderer/RenderModule.hpp"


namespace siren::editor
{

SceneViewRenderer::SceneViewRenderer()
{
    createEditorGrid();
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
    renderer.beginPass(frameBuffer, glm::vec4{ 0.14, 0.14, 0.14, 1 });

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

    // editor grid lines
    if (m_editorGrid.enabled) {
        glm::mat4 modelTransform{ 1 };
        modelTransform = translate(modelTransform, glm::vec3(camera->getPosition().x, 0, camera->getPosition().z));
        renderer.submit(m_editorGrid.mesh, m_editorGrid.material, m_editorGrid.shader, modelTransform);
    }

    renderer.endFrame();
}

void SceneViewRenderer::createEditorGrid()
{
    m_editorGrid.material              = createRef<core::Material>("Editor Grid Material");
    m_editorGrid.material->doubleSided = true;
    m_editorGrid.material->baseColor   = glm::vec4(0, 0, 0, 0);
    m_editorGrid.material->alphaMode   = core::Material::AlphaMode::BLEND;

    m_editorGrid.mesh = core::primitive::generatePlane(
        { .width = 1000, .depth = 1000, .widthSegments = 1, .depthSegments = 1 }
    );
    m_editorGrid.transform = glm::mat4{ 1 };
    m_editorGrid.enabled   = true;

    const auto& fs                = core::filesystem();
    const auto& shaderPath        = fs.getEngineRoot() / "assets" / "shaders";
    const std::string& vertSource = fs.readFile(shaderPath / "basic.vert");
    const std::string& fragSource = fs.readFile(shaderPath / "grid.frag");

    m_editorGrid.shader = createRef<core::Shader>("Editor Grid Shader", vertSource, fragSource);
}

} // namespace siren::editor
