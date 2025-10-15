#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "ecs/Components.hpp"
#include "ecs/Systems.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "events/KeyEvent.hpp"
#include "geometry/Mesh.hpp"
#include "scripts/PlayerController.hpp"
#include "scripts/ThirdPersonCamera.hpp"

namespace siren
{

TestLayer::TestLayer()
{
    utilities::UUID::setSeed(1); // useful for debugging

    auto& am                   = core::Application::get().getAssetManager();
    const core::Window& window = core::Application::get().getWindow();

    // load models
    const Maybe<assets::AssetHandle> playerResult =
        am.importAsset("ass://models/gltf/capsule/capsule.gltf");
    const Maybe<assets::AssetHandle> planeResult =
        am.importAsset("ass://models/gltf/plane/plane.gltf");
    if (!playerResult || !planeResult) { err("Could not load asset"); }

    // create hierarchy nodes to attach many scripts to a single entity implicitly
    const ecs::EntityHandle playerEntity = m_scene.create();

    m_scene.emplace<ecs::ModelComponent>(playerEntity, *playerResult);
    m_scene.emplace<ecs::TransformComponent>(playerEntity).scale *= 0.3;

    m_scene.bind<PlayerController>(playerEntity);
    m_scene.bind<ThirdPersonCamera>(playerEntity);

    auto& cc = m_scene.emplace<ecs::ThirdPersonCameraComponent>(
        playerEntity, window.getSize().x, window.getSize().y);

    const auto plane = m_scene.create();
    m_scene.emplace<ecs::ModelComponent>(plane, *planeResult);
    m_scene.emplace<ecs::TransformComponent>(plane);

    // tell renderer what to draw from
    m_scene.emplaceSingleton<ecs::RenderContextComponent>(&cc);

    // init core scene systems
    m_scene.start<ecs::RenderSystem>(ecs::RENDER_PHASE);
    m_scene.start<ecs::ScriptSystem>(ecs::SCRIPT_PHASE);
}

void TestLayer::onAttach()
{
}

void TestLayer::onDetach()
{
}

void TestLayer::onUpdate(const float delta)
{
    m_scene.onUpdate(delta);
}

void TestLayer::onRender()
{
    m_scene.onRender();
}

void TestLayer::onEvent(events::Event& e)
{
    events::EventHandler inputHandler(e);

    inputHandler.handle<events::KeyPressEvent>([this](const events::KeyPressEvent& keyPress) {
        auto& am = core::Application::get().getAssetManager();
        if (keyPress.getKeycode() == core::KeyCode::F2) {
            // reload shaders
            if (!am.reloadAsset(m_shaderHandle)) { err("Could not reload shaders"); }
        } else if (keyPress.getKeycode() == core::KeyCode::ESC) {
            core::Application::get().getWindow().close();
        }
        return true;
    });
}

} // namespace siren
