#include "EditorApp.hpp"

#include <imgui_internal.h>

#include "assets/AssetModule.hpp"
#include "renderer/RenderModule.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "core/App.hpp"
#include "panel/InspectorPanel.hpp"
#include "panel/SceneHierarchyPanel.hpp"
#include "panel/SceneViewPanel.hpp"
#include "ecs/Components.hpp"
#include "utilities/ImGui.hpp"
#include "ui/UI.hpp"


namespace siren::editor
{
void EditorApp::Init()
{
    s_instance->RegisterModule<core::FileSystemModule>();
    s_instance->RegisterModule<core::AssetModule>();
    s_instance->RegisterModule<core::RenderModule>();

    setupEditor();
}

void EditorApp::OnUpdate(const float delta)
{
    const bool res = core::input().isKeyPressed(core::KeyCode::F1);
    if (res) {
        core::Assets().ReloadAssetType(core::AssetType::Shader);
    }

    // todo: enum for play state
    // m_state->scene.onUpdate(delta); // dont update scene for now

    for (const auto& panel : m_panels) { panel->onUpdate(delta); }
}

void EditorApp::OnRender()
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
        const core::AssetHandle skyBoxRes = am.Import(fs.getAssetsRoot() / "cubemaps" / "skybox" / "sky.cube");

        const core::EntityHandle skybox = scene.Create();
        SirenAssert(skyBoxRes, "SkyBox Import failed");
        rcc.skyBoxComponent = &scene.Emplace<core::SkyLightComponent>(skybox, skyBoxRes);

        // todo: load scene from scene file
    }
}

EditorApp::~EditorApp()
{
    UI::shutdown();
}
} // namespace siren::editor
