#pragma once

#include "Panel.hpp"
#include "EditorCamera.hpp"
#include "SceneViewRenderer.hpp"
#include "ecs/core/Scene.hpp"
#include "renderer/FrameBuffer.hpp"
#include "utilities/spch.hpp"
#include "widget/EditorCameraPropertiesWidget.hpp"


namespace siren::editor
{

class SceneViewPanel final : public Panel
{
public:
    explicit SceneViewPanel(EditorState* state);
    ~SceneViewPanel() override = default;

    /// @brief The FrameBuffer attachment to be displayed
    enum class ViewMode { COLOR, DEPTH, STENCIL } m_viewMode = ViewMode::COLOR;

    void onUpdate(float delta) override;
    void draw() override;

    std::string getName() const override { return "Scene View"; }

private:
    SceneViewRenderer m_sceneViewRenderer{ };
    Ref<EditorCamera> m_editorCamera     = nullptr; // todo: maybe not use heap here?
    Ref<core::FrameBuffer> m_frameBuffer = nullptr; // todo: same here
    bool m_isMouseHovered                = false;   // kinda hacky
    bool m_isInteracting                 = false;   // prevent flickering when capturing mouse

    // ======== Nested UI =========
    Own<EditorCameraPropertiesWidget> m_cameraProperties = nullptr;

    void handleResize() const;
};

} // namespace siren::editor
