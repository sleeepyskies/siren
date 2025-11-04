#include "EditorLayer.hpp"

#include "ecs/Components.hpp"
#include "ui/ui.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "assets/AssetModule.hpp"
#include "utilities/ImGui.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "EditorContextComponent.hpp"
#include "utilities/spch.hpp"


namespace siren::editor
{

void EditorLayer::onAttach()
{
    ui::initUI(core::window());
    m_dockSpace = createOwn<DockSpace>(m_scene);

    auto& am       = core::assets();
    const auto& fs = core::filesystem();

    auto& rcc                         = m_scene->emplaceSingleton<core::RenderContextComponent>();
    const core::AssetHandle skyBoxRes = am.importAsset(fs.getAssetsRoot() / "cubemaps" / "skybox" / "sky.cube");

    const core::EntityHandle skybox = m_scene->create();
    SirenAssert(skyBoxRes, "SkyBox Import failed");
    rcc.skyBoxComponent = &m_scene->emplace<core::SkyLightComponent>(skybox, skyBoxRes);

    // todo: load scene from scene file
}

void EditorLayer::onDetach()
{
    ui::shutdownUI();
}

void EditorLayer::onUpdate(const float delta)
{
    // check state here? maybe not always call onUpdate, like if paused etc etc
    // todo: enum for play state
    m_scene->onUpdate(delta);

    // todo: only update panels if they should actually receive input
    m_dockSpace->onUpdate(delta); // basically only for SceneView
}

void EditorLayer::onRender()
{
    m_dockSpace->onRender();
}

void EditorLayer::onUiRender()
{
    ui::beginUI();
    m_mainMenuBar.onUiRender();

    m_dockSpace->onUiRender();
    ui::endUI();
}

} // namespace siren::editor
