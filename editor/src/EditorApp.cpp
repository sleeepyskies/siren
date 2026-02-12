#include "EditorApp.hpp"

#include <imgui_internal.h>

#include "assets/AssetModule.hpp"
#include "renderer/Renderer.hpp"
#include "../../core/src/core/file_system.hpp"
#include "core/app.hpp"
#include "panel/InspectorPanel.hpp"
#include "panel/SceneHierarchyPanel.hpp"
#include "panel/SceneViewPanel.hpp"
#include "ecs/Components.hpp"
#include "utilities/ImGui.hpp"
#include "ui/UI.hpp"


namespace siren::editor
{
void EditorApp::init()
{
    s_instance->RegisterModule<core::FileSystemModule>();
    s_instance->RegisterModule<core::AssetModule>();
    s_instance->RegisterModule<core::Renderer>();

    setupEditor();
}

void EditorApp::on_update(const float delta)
{
    const bool res = core::input().isKeyPressed(core::KeyCode::F1);
    if (res) {
        core::Assets().ReloadAssetType(core::AssetType::Shader);
    }

    // todo: enum for play state
    // m_state->scene.onUpdate(delta); // dont update scene for now

    for (const auto& panel : m_panels) { panel->onUpdate(delta); }
}

void EditorApp::on_render()
{
    UI::begin();
    ImGui::DockSpaceOverViewport(
        0,
        ImGui::GetMainViewport(),
        ImGuiDockNodeFlags_NoCloseButton
    );

    m_mainMenuBar.onRender();
    for (const auto& panel : m_panels) { panel->onRender(); }

    UI::end();
}

void EditorApp::setupEditor()
{
    UI::initialize();
    // todo: read from some save file how we want to setup UI, but for now just hardcode
    m_panels.emplace_back(create_own<SceneViewPanel>(m_state.get()));
    m_panels.emplace_back(create_own<SceneHierarchyPanel>(m_state.get()));
    m_panels.emplace_back(create_own<InspectorPanel>(m_state.get()));

    // create default testing scene
    {
        auto& am       = core::Assets();
        const auto& fs = core::filesystem();
        auto& scene    = m_state->scene;

        auto& rcc                         = scene.emplaceSingleton<core::RenderContextComponent>();
        const core::StrongHandle skyBoxRes = am.Import(fs.getAssetsRoot() / "cubemaps" / "skybox" / "sky.cube");

        const core::EntityHandle skybox = scene.Create();
        SIREN_ASSERT(skyBoxRes, "SkyBox Import failed");
        rcc.skyBoxComponent = &scene.Emplace<core::SkyLightComponent>(skybox, skyBoxRes);

        // todo: load scene from scene file
    }
}

EditorApp::~EditorApp()
{
    UI::shutdown();
}
} // namespace siren::editor
