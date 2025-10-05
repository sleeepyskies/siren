#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "events/KeyEvent.hpp"
#include "events/WindowEvent.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"

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
    const fs::path modelPath  = assetDir / "models" / "gltf" / "car" / "scene.gltf";
    const Maybe<assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const Maybe<assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    if (!shaderRes || !modelRes) { err("Could not load scene or model"); }
    m_shaderHandle = *shaderRes;
    m_modelHandle  = *modelRes;

    auto model = am.getAsset<geometry::Model>(m_modelHandle);
    model->scale(1);

    // setup hardcoded uniform buffer with hardcoded lights
    GPULightPoints gpuLightPoints{};
    std::vector<geometry::PointLight> pointLights{};
    geometry::PointLight l0;
    l0.color    = glm::vec3{ 1, 0, 0 };
    l0.position = glm::vec3{ 3, 1.75, 0 };
    geometry::PointLight l1;
    l1.color    = glm::vec3{ 0, 1, 0 };
    l1.position = glm::vec3{ -2, 1.3, -1.3 };
    geometry::PointLight l2;
    l2.color    = glm::vec3{ 0, 0, 1 };
    l2.position = glm::vec3{ 0, 0, 2.3 };

    gpuLightPoints.lights[0]  = l0;
    gpuLightPoints.lights[1]  = l1;
    gpuLightPoints.lights[2]  = l2;
    gpuLightPoints.lightCount = 3;
    m_pointLights->uploadData(toBytesPod(gpuLightPoints), renderer::STATIC, 0);
}

void TestLayer::onAttach()
{
}

void TestLayer::onDetach()
{
    renderer::Renderer::shutdown();
}

void TestLayer::onUpdate(const float delta)
{
    m_camera.onUpdate(delta);
}

void TestLayer::onRender()
{
    renderer::SceneDescription sceneDescription;
    sceneDescription.camera      = makeRef<geometry::Camera>(m_camera);
    sceneDescription.pointLights = m_pointLights;
    renderer::Renderer::beginScene(sceneDescription);

    const assets::AssetManager& am = core::Application::get().getAssetManager();

    const auto& shader = am.getAsset<renderer::Shader>(m_shaderHandle);
    const auto& model  = am.getAsset<geometry::Model>(m_modelHandle);

    for (const auto& mesh : model->getMeshes()) {
        const auto& material = mesh.getMaterial();
        if (!material) { continue; }
        glm::mat4 transform = model->getGlobalTransform() * mesh.getLocalTransform();
        renderer::Renderer::draw(mesh.getVertexArray(), material, transform, shader);
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
        auto& am             = core::Application::get().getAssetManager();
        auto model           = am.getAsset<geometry::Model>(m_modelHandle);
        const float moveAmnt = 0.3;
        if (keyPress.getKeycode() == core::KeyCode::F1) {
            glm::vec3 pos = m_camera.position();
            nfo("Current coordinates: ({} ,{}, {})", pos.x, pos.y, pos.z);
        } else if (keyPress.getKeycode() == core::KeyCode::F2) {
            // reload shaders
            if (!am.reloadAsset(m_shaderHandle)) { err("Could not reload shaders"); }
        } else if (keyPress.getKeycode() == core::KeyCode::ARROW_LEFT) {
            model->translate(glm::vec3{ -1, 0, 0 }, moveAmnt);
        } else if (keyPress.getKeycode() == core::KeyCode::ARROW_RIGHT) {
            model->translate(glm::vec3{ 1, 0, 0 }, moveAmnt);
        } else if (keyPress.getKeycode() == core::KeyCode::ARROW_UP) {
            model->translate(glm::vec3{ 0, 1, 0 }, moveAmnt);
        } else if (keyPress.getKeycode() == core::KeyCode::ARROW_DOWN) {
            model->translate(glm::vec3{ 0, -1, 0 }, moveAmnt);
        } else if (keyPress.getKeycode() == core::KeyCode::PAGE_UP) {
            model->translate(glm::vec3{ 0, 0, 1 }, moveAmnt);
        } else if (keyPress.getKeycode() == core::KeyCode::PAGE_DOWN) {
            model->translate(glm::vec3{ 0, 0, -1 }, moveAmnt);
        }
        return true;
    });
}

} // namespace siren
