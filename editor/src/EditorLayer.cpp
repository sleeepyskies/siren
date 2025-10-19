#include "EditorLayer.hpp"

#include "EditorApp.hpp"
#include "ecs/Components.hpp"
#include "ecs/Systems.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "ui/ImGui.hpp"

// todo: this should be removed once we have serialization
#include "scripts/PlayerController.hpp"
#include "scripts/ThirdPersonCamera.hpp"

namespace siren::editor
{

void EditorLayer::onAttach()
{
    // todo: load scene from scene file
    utilities::UUID::setSeed(1); // useful for debugging

    auto& am                   = EditorApp::get().getAssetManager();
    const core::Window& window = EditorApp::get().getWindow();

    // load models
    const Maybe<assets::AssetHandle> playerResult =
        am.importAsset("ass://models/gltf/capsule/capsule.gltf");
    const Maybe<assets::AssetHandle> planeResult =
        am.importAsset("ass://models/gltf/plane/plane.gltf");
    if (!playerResult || !planeResult) { err("Could not load asset"); }

    // create hierarchy nodes to attach many scripts to a single entity implicitly
    const ecs::EntityHandle playerEntity = m_scene->create();

    m_scene->emplace<ecs::ModelComponent>(playerEntity, *playerResult);
    m_scene->emplace<ecs::TransformComponent>(playerEntity).scale *= 0.3;
    m_scene->emplace<ecs::TagComponent>(playerEntity, "Player");

    m_scene->bind<PlayerController>(playerEntity);
    m_scene->bind<ThirdPersonCamera>(playerEntity);

    auto& cc = m_scene->emplace<ecs::ThirdPersonCameraComponent>(
        playerEntity, window.getSize().x, window.getSize().y);

    const auto plane = m_scene->create();
    m_scene->emplace<ecs::TagComponent>(plane, "Plane");
    m_scene->emplace<ecs::ModelComponent>(plane, *planeResult);
    m_scene->emplace<ecs::TransformComponent>(plane);

    const auto pointLight = m_scene->create();
    m_scene->emplace<ecs::PointLightComponent>(pointLight, glm::vec3{ 0, 1, 0 }, glm::vec3{ 1 });
    m_scene->emplace<ecs::TagComponent>(pointLight, "PointLight");

    // tell renderer what to draw from
    m_scene->emplaceSingleton<ecs::RenderContextComponent>(&cc);

    // init core scene systems
    // do we want this in the editor?
    // m_scene->start<ecs::ScriptSystem>(ecs::SCRIPT_PHASE);
}

void EditorLayer::onDetach()
{
}

void EditorLayer::onUpdate(const float delta)
{
    // check state here? maybe not always call onUpdate, like if paused etc etc
    // todo: enum for play state
    m_scene->onUpdate(delta);

    // todo: only update panels if they should actually receive input
    m_dockSpace.onUpdate(delta); // basically only for SceneView
}

void EditorLayer::onRender()
{
    m_dockSpace.onRender();
}

void EditorLayer::onUiRender()
{
    m_mainMenuBar.onUiRender();

    m_dockSpace.onUiRender();
}

void EditorLayer::onEvent(event::Event& e)
{
}

std::string EditorLayer::getName() const
{
    return "EditorLayer";
}

} // namespace siren::editor
