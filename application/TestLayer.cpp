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

// for uniform buffer
struct GPULightPoints {
    geometry::PointLight lights[16];
    int lightCount;
    int _pad[3];
};

TestLayer::TestLayer()
{
    auto& am                   = core::Application::get().getAssetManager();
    const core::Window& window = core::Application::get().getWindow();
    const fs::path& workingDir = core::Application::get().getProperties().workingDirectory;
    const fs::path assetDir    = workingDir / "assets";

    // load shaders and model
    const fs::path shaderPath = assetDir / "shaders" / "basic.sshg";
    const fs::path modelPath  = assetDir / "models" / "gltf" / "nightmares" / "scene.gltf";
    const fs::path envPath    = assetDir / "models" / "gltf" / "house" / "scene.gltf";
    const Maybe<assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const Maybe<assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    const Maybe<assets::AssetHandle> envRes    = am.importAsset(envPath);
    if (!shaderRes || !modelRes || !envRes) { err("Could not load scene or model"); }

    m_shaderHandle = *shaderRes;

    m_scene.add<ecs::ModelComponent>(m_cameraEntity);
    m_scene.add<ecs::TransformComponent>(m_cameraEntity);
    m_scene.add<ecs::CameraComponent>(m_cameraEntity);
    m_scene.add<ecs::PlayerComponent>(m_cameraEntity);
    auto& mc         = m_scene.get<ecs::ModelComponent>(m_cameraEntity);
    auto& tc         = m_scene.get<ecs::TransformComponent>(m_cameraEntity);
    auto& cc         = m_scene.get<ecs::CameraComponent>(m_cameraEntity);
    auto& pc         = m_scene.get<ecs::PlayerComponent>(m_cameraEntity);
    pc.movementSpeed = 3.f;
    tc.scale         = glm::vec3{ 0.1 };
    mc.modelHandle   = *modelRes;
    cc.fov           = 75;
    cc.far           = 1000.f;
    cc.near          = 0.1f;
    cc.width         = window.getSize().x;
    cc.height        = window.getSize().y;
    cc.position      = glm::vec3{ 0, 0, 3 };
    cc.viewDirection = glm::normalize(-cc.position);
    cc.sensitivity   = 5.f;

    auto e2 = m_scene.create();
    m_scene.add<ecs::ModelComponent>(e2);
    m_scene.add<ecs::TransformComponent>(e2);
    auto& mc2       = m_scene.get<ecs::ModelComponent>(e2);
    auto& tc2       = m_scene.get<ecs::TransformComponent>(e2);
    tc2.scale       = glm::vec3{ 0.1 };
    tc2.position    = glm::vec3{ 0, 0, -10 };
    mc2.modelHandle = *envRes;

    m_scene.start<ecs::PlayerControllerSystem>();
    m_scene.start<ecs::CameraSystem>();
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
    auto& cc = m_scene.get<ecs::CameraComponent>(m_cameraEntity);
    renderer::Renderer::beginScene(cc);

    const assets::AssetManager& am = core::Application::get().getAssetManager();

    const auto& shader = am.getAsset<renderer::Shader>(m_shaderHandle);

    for (const auto& e : m_scene.getWith<ecs::TransformComponent, ecs::ModelComponent>()) {

        auto& tc = m_scene.get<ecs::TransformComponent>(e);
        auto& mc = m_scene.get<ecs::ModelComponent>(e);

        const glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.position) *
                                    glm::mat4_cast(tc.rotation) *
                                    glm::scale(glm::mat4(1.0f), tc.scale);

        const auto model = am.getAsset<geometry::Model>(mc.modelHandle);
        for (const auto& meshHandle : model->getMeshHandles()) {
            const Ref<geometry::Mesh>& mesh         = am.getAsset<geometry::Mesh>(meshHandle);
            const Ref<geometry::Material>& material = mesh->getMaterial();
            glm::mat4 transform2                    = transform * mesh->getLocalTransform();
            renderer::Renderer::draw(mesh->getVertexArray(), material, transform2, shader);
        }
    }

    renderer::Renderer::endScene();
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
