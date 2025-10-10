#include "SceneRenderer.hpp"

#include "Renderer.hpp"
#include "core/Application.hpp"
#include "ecs/Components.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Shader.hpp"

namespace siren::renderer
{

void SceneRenderer::setActiveCamera(const ecs::EntityHandle cameraHandle)
{
    m_activeCameraEntity = cameraHandle;
}

void SceneRenderer::setActiveShader(const assets::AssetHandle handle)
{
    m_activeShaderHandle = handle;
}

void SceneRenderer::draw(ecs::Scene& scene) const
{
    const auto& am = core::Application::get().getAssetManager();

    // assumes there is just one camera
    if (!m_activeCameraEntity || !m_activeShaderHandle) { return; }
    const auto camera = scene.getSafe<ecs::CameraComponent>(m_activeCameraEntity);
    const auto shader = am.getAsset<Shader>(m_activeShaderHandle);
    if (!camera || !shader) { return; }

    CameraProperties camProps{ .projectionViewMatrix = camera->projectionViewMatrix(),
                               .position             = camera->position };

    Renderer::beginScene(camProps);

    for (const auto& e : scene.getWith<ecs::ModelComponent, ecs::TransformComponent>()) {
        const auto modelComponent     = scene.getSafe<ecs::ModelComponent>(e);
        const auto transformComponent = scene.getSafe<ecs::TransformComponent>(e);
        if (!modelComponent || !transformComponent) { continue; }
        const auto model     = am.getAsset<geometry::Model>(modelComponent->modelHandle);
        const auto transform = transformComponent->getTransform();

        for (const auto& meshHandle : model->getMeshHandles()) {
            const auto mesh               = am.getAsset<geometry::Mesh>(meshHandle);
            const glm::mat4 meshTransform = transform * mesh->getLocalTransform();
            Renderer::draw(mesh->getVertexArray(), mesh->getMaterial(), meshTransform, shader);
        }
    }

    Renderer::endScene();
}

} // namespace siren::renderer
