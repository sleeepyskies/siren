#include "EditorLayer.hpp"

#include "ecs/Components.hpp"
#include "ui/ui.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "assets/AssetModule.hpp"
#include "utilities/ImGui.hpp"

// todo: this should be removed once we have serialization
#include "scripts/PlayerController.hpp"
#include "scripts/ThirdPersonCamera.hpp"

#include "window/WindowModule.hpp"


namespace siren::editor
{

void EditorLayer::onAttach()
{
    ui::initUI(core::window());

    m_dockSpace = createOwn<DockSpace>(m_scene);

    // todo: load scene from scene file

    auto& am                         = core::assets();
    const core::WindowModule& window = core::window();

    // load models
    const Maybe<core::AssetHandle> playerResult =
            am.createPrimitive(core::CapsuleParams{ .radius = 1, .height = 2, .segments = 16 });
    const Maybe<core::AssetHandle> planeResult =
            am.createPrimitive(core::PlaneParams{ .width = 50, .depth = 50, .widthSegments = 1, .depthSegments = 1 });
    if (!playerResult || !planeResult) { err("Could not load asset"); }

    // create hierarchy nodes to attach many scripts to a single entity implicitly
    const core::EntityHandle playerEntity = m_scene->create();

    m_scene->emplace<core::MeshComponent>(playerEntity, *playerResult);
    m_scene->emplace<core::TransformComponent>(playerEntity).scale *= 0.3;
    m_scene->emplace<core::TagComponent>(playerEntity, "Player");

    m_scene->bind<PlayerController>(playerEntity);
    m_scene->bind<ThirdPersonCamera>(playerEntity);

    auto& cc = m_scene->emplace<core::ThirdPersonCameraComponent>(
        playerEntity,
        window.getSize().x,
        window.getSize().y
    );

    const auto plane = m_scene->create();
    m_scene->emplace<core::TagComponent>(plane, "Plane");
    m_scene->emplace<core::MeshComponent>(plane, *planeResult);
    m_scene->emplace<core::TransformComponent>(plane);

    const auto pointLight = m_scene->create();
    m_scene->emplace<core::PointLightComponent>(pointLight, glm::vec3{ 0, 1, 0 }, glm::vec3{ 1 });
    m_scene->emplace<core::TagComponent>(pointLight, "PointLight");

    // tell renderer what to draw from
    m_scene->emplaceSingleton<core::RenderContextComponent>(&cc);

    // init core scene systems
    // do we want this in the editor?
    // m_scene->start<ecs::ScriptSystem>(ecs::SCRIPT_PHASE);
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
