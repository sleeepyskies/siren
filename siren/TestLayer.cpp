#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "ecs/Components.hpp"
#include "ecs/Systems.hpp"
#include "events/KeyEvent.hpp"
#include "events/WindowEvent.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"

#include <core/Input.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren
{

TestLayer::TestLayer()
{
    utilities::UUID::setSeed(1);

    auto& am                   = core::Application::get().getAssetManager();
    const core::Window& window = core::Application::get().getWindow();
    const fs::path& workingDir = core::Application::get().getProperties().workingDirectory;
    const fs::path assetDir    = workingDir / "assets";

    // load shaders and model
    const fs::path shaderPath = assetDir / "shaders" / "basic.sshg";
    const fs::path modelPath  = assetDir / "models" / "gltf" / "stick_man" / "scene.gltf";
    const fs::path envPath    = assetDir / "models" / "gltf" / "house" / "scene.gltf";
    const fs::path planePath  = assetDir / "models" / "gltf" / "plane" / "plane.gltf";
    const Maybe<assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const Maybe<assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    const Maybe<assets::AssetHandle> envRes    = am.importAsset(envPath);
    const Maybe<assets::AssetHandle> planeRes  = am.importAsset(planePath);
    if (!shaderRes || !modelRes || !envRes || !planeRes) { err("Could not load asset"); }

    m_shaderHandle = *shaderRes;

    m_scene.emplace<ecs::ModelComponent>(m_cameraEntity, *modelRes);

    auto& tc = m_scene.emplace<ecs::TransformComponent>(m_cameraEntity);
    tc.scale = glm::vec3{ 0.1 };

    auto& cc         = m_scene.emplace<ecs::CameraComponent>(m_cameraEntity);
    cc.fov           = 75;
    cc.far           = 1000.f;
    cc.near          = 0.1f;
    cc.width         = window.getSize().x;
    cc.height        = window.getSize().y;
    cc.position      = glm::vec3{ 0, 0, 3 };
    cc.viewDirection = glm::normalize(-cc.position);
    cc.sensitivity   = 5.f;

    auto& pc         = m_scene.emplace<ecs::PlayerComponent>(m_cameraEntity);
    pc.movementSpeed = 3.f;

    auto e2 = m_scene.create();
    m_scene.emplace<ecs::ModelComponent>(e2, *envRes);
    auto& tc2    = m_scene.emplace<ecs::TransformComponent>(e2);
    tc2.scale    = glm::vec3{ 0.1 };
    tc2.position = glm::vec3{ 0, 0, -10 };

    const auto plane = m_scene.create();
    m_scene.emplace<ecs::ModelComponent>(plane, *planeRes);
    m_scene.emplace<ecs::TransformComponent>(plane);

    m_scene.start<ecs::PlayerControllerSystem>();
    m_scene.start<ecs::CameraSystem>();
    m_sceneRenderer.setActiveCamera(m_cameraEntity);
    m_sceneRenderer.setActiveShader(m_shaderHandle);
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
    m_sceneRenderer.draw(m_scene);
}

void TestLayer::onEvent(events::Event& e)
{
    events::EventHandler resizeHandler(e);
    events::EventHandler inputHandler(e);

    resizeHandler.handle<events::WindowResizeEvent>(
        [this](const events::WindowResizeEvent& windowResize) {
            /*
            m_camera.setViewportWidth(windowResize.getWidth());
            m_camera.setViewportHeight(windowResize.getHeight());
            */
            return false; // do not consume resize events
        });

    inputHandler.handle<events::KeyPressEvent>([this](const events::KeyPressEvent& keyPress) {
        auto& am = core::Application::get().getAssetManager();
        if (keyPress.getKeycode() == core::KeyCode::F1) {
            // glm::vec3 pos = m_camera.position();
            // nfo("Current coordinates: ({} ,{}, {})", pos.x, pos.y, pos.z);
        } else if (keyPress.getKeycode() == core::KeyCode::F2) {
            // reload shaders
            if (!am.reloadAsset(m_shaderHandle)) { err("Could not reload shaders"); }
        } else if (keyPress.getKeycode() == core::KeyCode::ESC) {
            core::Application::get().getWindow().close();
        }
        return true;
    });
}

} // namespace siren
