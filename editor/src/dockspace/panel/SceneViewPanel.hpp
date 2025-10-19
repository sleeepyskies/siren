#pragma once

#include "DockPanel.hpp"
#include "EditorCamera.hpp"
#include "SceneViewRenderer.hpp"
#include "ecs/core/Scene.hpp"
#include "renderer/FrameBuffer.hpp"
#include "utilities/spch.hpp"
#include "widget/EditorCameraPropertiesWidget.hpp"

namespace siren::editor
{

class SceneViewPanel final : public DockPanel
{
public:
    explicit SceneViewPanel(const Ref<ecs::Scene>& scene);

    /// @brief The FrameBuffer attachment to be displayed
    enum class ViewMode { COLOR, DEPTH, STENCIL } m_viewMode = ViewMode::COLOR;

    void onUpdate(float delta) override;
    void onRender() const override;
    void onUiRender() override;
    std::string getName() const override;

private:
    std::string m_name = "Scene View";
    SceneViewRenderer m_sceneViewRenderer{};
    Ref<ecs::Scene> m_scene                  = nullptr;
    Ref<EditorCamera> m_editorCamera         = nullptr; // todo: maybe not use heap here?
    Ref<renderer::FrameBuffer> m_frameBuffer = nullptr; // todo: same here
    bool m_isMouseHovered                    = false;   // kinda hacky
    bool m_isInteracting                     = false;   // prevent flickering when captuing mouse

    // ======== Nested UI =========
    Uref<EditorCameraPropertiesWidget> m_cameraProperties = nullptr;

    void handleResize() const;
};

} // namespace siren::editor
