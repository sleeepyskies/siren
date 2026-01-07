#include "SceneViewPanel.hpp"

#include "EditorCamera.hpp"
#include "ecs/Components.hpp"
#include "renderer/RenderModule.hpp"
#include "utilities/ImGui.hpp"


namespace siren::editor
{

SceneViewPanel::SceneViewPanel(EditorState* state) : Panel(state)
{
    core::FrameBuffer::Properties frameBufferProperties{
        .width = 1280,
        .height = 720,
        .hasColorBuffer = true,
        .hasDepthBuffer = true
    };
    m_frameBuffer  = create_ref<core::FrameBuffer>(frameBufferProperties);
    m_editorCamera = create_ref<EditorCamera>(
        m_frameBuffer->getProperties().width,
        m_frameBuffer->getProperties().height
    );
    m_cameraProperties = create_own<EditorCameraPropertiesWidget>(m_editorCamera->getProperties());
}

void SceneViewPanel::onUpdate(const float delta)
{
    if (!m_isMouseHovered && !m_isInteracting) { return; }
    m_isInteracting = m_editorCamera->onUpdate(delta);
}

void SceneViewPanel::draw()
{
    if (!m_frameBuffer) {
        err("No FrameBuffer for SceneViewRenderer to use!");
        return;
    }
    m_sceneViewRenderer.render(m_state->scene, m_editorCamera, m_frameBuffer);

    handleResize();

    m_cameraProperties->onRender();

    const auto attachmentResult = m_frameBuffer->GetColorAttachmentID();
    if (attachmentResult) {
        const u32 attachment = *attachmentResult;
        ImGui::Image(attachment, ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    }

    m_isMouseHovered = ImGui::IsItemHovered();
}

void SceneViewPanel::handleResize() const
{
    // todo: we can only resize the framebuffer during ui render, may be out of sync?
    const ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    if (availableSpace.x <= 0 || availableSpace.y <= 0) { return; } // cant make 0 dimension
    const auto& fbSpecs = m_frameBuffer->getProperties();
    if (availableSpace.x != fbSpecs.width || availableSpace.y != fbSpecs.height) {
        m_frameBuffer->Resize(availableSpace.x, availableSpace.y);
        m_editorCamera->onResize(availableSpace.x, availableSpace.y);
    }
}

} // namespace siren::editor
