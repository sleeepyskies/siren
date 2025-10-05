#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "ecs/Components.hpp"
#include "events/KeyEvent.hpp"
#include "events/WindowEvent.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"

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
    : m_camera(core::Application::get().getWindow().getSize().x,
               core::Application::get().getWindow().getSize().y)
{
    auto& am                   = core::Application::get().getAssetManager();
    const fs::path& workingDir = core::Application::get().getProperties().workingDirectory;
    const fs::path assetDir    = workingDir / "assets";

    // load shaders and model
    const fs::path shaderPath = assetDir / "shaders" / "basic.sshg";
    const fs::path modelPath  = assetDir / "models" / "gltf" / "nightmares" / "scene.gltf";
    const Maybe<assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const Maybe<assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    if (!shaderRes || !modelRes) { err("Could not load scene or model"); }

    m_shaderHandle = *shaderRes;

    secs::Entity e1 = m_scene.createEntity();
    ecs::ModelComponent mc1;
    ecs::TransformComponent tc1{};
    tc1.transform   = glm::scale(tc1.transform, glm::vec3{ 0.1 });
    mc1.modelHandle = *modelRes;
    m_scene.registerComponent(e1, mc1);
    m_scene.registerComponent(e1, tc1);
}

void TestLayer::onAttach()
{
}

void TestLayer::onDetach()
{
}

void TestLayer::onUpdate(const float delta)
{
    m_camera.onUpdate(delta);
    m_scene.onUpdate(delta);
}

void TestLayer::onRender()
{
    renderer::SceneDescription sceneDescription;
    sceneDescription.camera = makeRef<geometry::Camera>(m_camera);
    renderer::Renderer::beginScene(sceneDescription);

    const assets::AssetManager& am = core::Application::get().getAssetManager();

    const auto& shader = am.getAsset<renderer::Shader>(m_shaderHandle);

    for (const auto& e :
         m_scene.getComponentEntities<ecs::TransformComponent, ecs::ModelComponent>()) {

        auto tc = m_scene.getComponent<ecs::TransformComponent>(e);
        auto mc = m_scene.getComponent<ecs::ModelComponent>(e);

        const auto model = am.getAsset<geometry::Model>(mc.modelHandle);
        for (const auto& meshHandle : model->getMeshHandles()) {
            const Ref<geometry::Mesh>& mesh         = am.getAsset<geometry::Mesh>(meshHandle);
            const Ref<geometry::Material>& material = mesh->getMaterial();
            glm::mat4 transform                     = tc.transform * mesh->getLocalTransform();
            renderer::Renderer::draw(mesh->getVertexArray(), material, transform, shader);
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
            m_camera.setViewportWidth(windowResize.getWidth());
            m_camera.setViewportHeight(windowResize.getHeight());
            return false; // do not consume resize events
        });

    inputHandler.handle<events::KeyPressEvent>([this](const events::KeyPressEvent& keyPress) {
        auto& am = core::Application::get().getAssetManager();
        if (keyPress.getKeycode() == core::KeyCode::F1) {
            glm::vec3 pos = m_camera.position();
            nfo("Current coordinates: ({} ,{}, {})", pos.x, pos.y, pos.z);
        } else if (keyPress.getKeycode() == core::KeyCode::F2) {
            // reload shaders
            if (!am.reloadAsset(m_shaderHandle)) { err("Could not reload shaders"); }
        }
        return true;
    });
}

} // namespace siren
