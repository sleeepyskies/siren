#pragma once

#include "DockPanel.hpp"
#include "EditorCamera.hpp"
#include "ecs/core/Scene.hpp"
#include "renderer/FrameBuffer.hpp"
#include "utilities/spch.hpp"

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
    std::string m_name                        = "Scene View";
    Ref<ecs::Scene> m_scene                   = nullptr;
    Uref<EditorCamera> m_editorCamera         = nullptr;
    Uref<renderer::FrameBuffer> m_frameBuffer = nullptr;
    bool m_isMouseHovered                     = false; // kinda hacky

    void handleResize() const;
};

} // namespace siren::editor
