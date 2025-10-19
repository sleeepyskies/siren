#include "SceneViewPanel.hpp"

#include "EditorCamera.hpp"
#include "ecs/Components.hpp"
#include "renderer/Renderer.hpp"
#include "ui/ImGui.hpp"

namespace siren::editor
{

SceneViewPanel::SceneViewPanel(const Ref<ecs::Scene>& scene) : m_scene(scene)
{
    renderer::FrameBuffer::Properties frameBufferProperties{ .width          = 1280,
                                                             .height         = 720,
                                                             .hasDepthBuffer = true };
    m_frameBuffer      = makeRef<renderer::FrameBuffer>(frameBufferProperties);
    m_editorCamera     = makeRef<EditorCamera>(m_frameBuffer->getProperties().width,
                                           m_frameBuffer->getProperties().height);
    m_cameraProperties = makeUref<EditorCameraPropertiesWidget>(m_editorCamera->getProperties());
}

void SceneViewPanel::onUpdate(const float delta)
{
    if (!m_isMouseHovered && !m_isInteracting) { return; }
    m_isInteracting = m_editorCamera->onUpdate(delta);
}

void SceneViewPanel::onRender() const
{
    m_sceneViewRenderer.render(m_scene, m_editorCamera, m_frameBuffer);
}

void SceneViewPanel::onUiRender()
{
    handleResize();

    m_cameraProperties->onUiRender();

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
    if (availableSpace.x != fbSpecs.width || availableSpace.y != fbSpecs.height) {
        m_frameBuffer->resize(availableSpace.x, availableSpace.y);
        m_editorCamera->onResize(availableSpace.x, availableSpace.y);
    }
}

} // namespace siren::editor