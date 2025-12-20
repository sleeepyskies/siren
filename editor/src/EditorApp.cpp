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
void EditorApp::initialize()
{
    s_instance->registerModule<core::FileSystemModule>();
    s_instance->registerModule<core::AssetModule>();
    s_instance->registerModule<core::RenderModule>();

    setupEditor();
}

void EditorApp::onUpdate(const float delta)
{
    if (core::input().isKeyPressed(core::KeyCode::F12)) {
        core::assets().reloadAssetType(core::AssetType::SHADER);
    }

    // todo: enum for play state
    // m_state->scene.onUpdate(delta); // dont update scene for now

    for (const auto& panel : m_panels) { panel->onUpdate(delta); }
}

void EditorApp::onRender()
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
    m_panels.emplace_back(createOwn<SceneViewPanel>(m_state.get()));
    m_panels.emplace_back(createOwn<SceneHierarchyPanel>(m_state.get()));
    m_panels.emplace_back(createOwn<InspectorPanel>(m_state.get()));

    // create default testing scene
    {
        auto& am       = core::assets();
        const auto& fs = core::filesystem();
        auto& scene    = m_state->scene;

        auto& rcc                         = scene.emplaceSingleton<core::RenderContextComponent>();
        const core::AssetHandle skyBoxRes = am.importAsset(fs.getAssetsRoot() / "cubemaps" / "skybox" / "sky.cube");

        const core::EntityHandle skybox = scene.create();
        SirenAssert(skyBoxRes, "SkyBox Import failed");
        rcc.skyBoxComponent = &scene.emplace<core::SkyLightComponent>(skybox, skyBoxRes);

        // todo: load scene from scene file
    }
}

EditorApp::~EditorApp()
{
    UI::shutdown();
}
} // namespace siren::editor
