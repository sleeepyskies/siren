#include "SceneViewPanel.hpp"

#include "EditorApp.hpp"
#include "EditorCamera.hpp"
#include "ecs/Components.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "ui/ImGui.hpp"

namespace siren::editor
{

SceneViewPanel::SceneViewPanel(const Ref<ecs::Scene>& scene) : m_scene(scene)
{
    renderer::FrameBuffer::Properties frameBufferProperties{ .width          = 1280,
                                                             .height         = 720,
                                                             .hasDepthBuffer = true };
    m_frameBuffer  = makeUref<renderer::FrameBuffer>(frameBufferProperties);
    m_editorCamera = makeUref<EditorCamera>(m_frameBuffer->getProperties().width,
                                            m_frameBuffer->getProperties().height);
}

void SceneViewPanel::onUpdate(const float delta)
{
    m_editorCamera->onUpdate(delta, m_isMouseHovered);
}

void SceneViewPanel::onRender() const
{
    // render the scene to our framebuffer
    m_frameBuffer->bind();
    m_frameBuffer->prepare();

    const auto& am = EditorApp::get().getAssetManager();

    // maybe make a EditorSceneRenderer class?
    renderer::CameraInfo cameraInfo{ m_editorCamera->getProjViewMat(),
                                     m_editorCamera->getPosition() };
    renderer::Renderer::begin(cameraInfo);

    // iterate over all drawable entities
    for (const auto& e : m_scene->getWith<ecs::ModelComponent, ecs::TransformComponent>()) {
        const auto* modelComponent     = m_scene->getSafe<ecs::ModelComponent>(e);
        const auto* transformComponent = m_scene->getSafe<ecs::TransformComponent>(e);

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

    m_frameBuffer->unbind();
}

void SceneViewPanel::onUiRender()
{
    handleResize();

    const auto attachmentResult = m_frameBuffer->getColorAttachmentId();
    if (attachmentResult) {
        const uint32_t attachment = *attachmentResult;
        ImGui::Image(attachment, ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    }

    m_isMouseHovered = ImGui::IsItemHovered();
}

std::string SceneViewPanel::getName() const
{
    return m_name;
}

void SceneViewPanel::handleResize() const
{
    // todo: we can only resize the framebuffer during ui render, may be out of sync?
    const ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    const auto& fbSpecs         = m_frameBuffer->getProperties();
    if (availableSpace.x != fbSpecs.width && availableSpace.y != fbSpecs.height) {
        m_frameBuffer->resize(availableSpace.x, availableSpace.y);
        m_editorCamera->setWidth(availableSpace.x);
        m_editorCamera->setHeight(availableSpace.y);
    }
}

} // namespace siren::editor