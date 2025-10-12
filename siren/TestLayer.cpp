#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "ecs/Components.hpp"
#include "ecs/Systems.hpp"
#include "events/KeyEvent.hpp"
#include "geometry/Mesh.hpp"

namespace siren
{

TestLayer::TestLayer()
{
    utilities::UUID::setSeed(1); // useful for debugging

    auto& am                   = core::Application::get().getAssetManager();
    const core::Window& window = core::Application::get().getWindow();
    const fs::path& workingDir = core::Application::get().getProperties().workingDirectory;
    const fs::path assetDir    = workingDir / "assets";

    // load shaders and model
    const fs::path shaderPath = assetDir / "shaders" / "basic.sshg";
    const fs::path playerPath = assetDir / "models" / "gltf" / "capsule" / "capsule.gltf";
    const fs::path envPath    = assetDir / "models" / "gltf" / "house" / "scene.gltf";
    const fs::path planePath  = assetDir / "models" / "gltf" / "plane" / "plane.gltf";
    const Maybe<assets::AssetHandle> playerRes = am.importAsset(playerPath);
    const Maybe<assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const Maybe<assets::AssetHandle> envRes    = am.importAsset(envPath);
    const Maybe<assets::AssetHandle> planeRes  = am.importAsset(planePath);
    if (!shaderRes || !playerRes || !envRes || !planeRes) { err("Could not load asset"); }

    m_shaderHandle = *shaderRes;

    ecs::EntityHandle playerEntity = m_scene.create();

    m_scene.emplace<ecs::ModelComponent>(playerEntity, *playerRes);

    auto& tc = m_scene.emplace<ecs::TransformComponent>(playerEntity);
    tc.scale = glm::vec3{ 0.1 };

    auto& cc          = m_scene.emplace<ecs::ThirdPersonCameraComponent>(playerEntity);
    cc.fov            = 75;
    cc.farPlane       = 1000.f;
    cc.nearPlane      = 0.1f;
    cc.viewportWidth  = window.getSize().x;
    cc.viewportHeight = window.getSize().y;
    cc.position       = glm::vec3{ 0, 0, 3 };
    cc.viewDirection  = glm::normalize(-cc.position);
    cc.sensitivity    = 5.f;

    auto& pc         = m_scene.emplace<ecs::PlayerComponent>(playerEntity);
    pc.movementSpeed = 3.f;

    auto e2 = m_scene.create();
    m_scene.emplace<ecs::ModelComponent>(e2, *envRes);
    auto& tc2    = m_scene.emplace<ecs::TransformComponent>(e2);
    tc2.scale    = glm::vec3{ 0.1 };
    tc2.position = glm::vec3{ 0, 0, -10 };

    const auto plane = m_scene.create();
    m_scene.emplace<ecs::ModelComponent>(plane, *planeRes);
    m_scene.emplace<ecs::TransformComponent>(plane);

    // tell renderer what to draw from
    m_scene.emplaceSingleton<ecs::RenderContextComponent>(&cc, *shaderRes);

    m_scene.start<ecs::PlayerControllerSystem>();
    m_scene.start<ecs::ThirdPersonCameraSystem>();
    m_scene.start<ecs::RenderSystem>();
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
