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
    core::PointLight lights[16];
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
    const core::Maybe<core::assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const core::Maybe<core::assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    if (!shaderRes || !modelRes) { err("Could not load scene or model"); }
    m_shaderHandle = *shaderRes;
    m_modelHandle  = *modelRes;

    auto model = am.getAsset<core::geometry::Model>(m_modelHandle);
    model->scale(0.1);

    // setup hardcoded uniform buffer with hardcoded lights
    GPULightPoints gpuLightPoints{};
    std::vector<core::PointLight> pointLights{};
    core::PointLight l0;
    l0.color    = glm::vec3{ 1 };
    l0.position = glm::vec3{ 3, 3, -3 };
    /*
    core::PointLight l1;
    l1.color    = glm::vec3{ 0.5 };
    l1.position = glm::vec3{ 9, 13, -7 };
    core::PointLight l2;
    l2.color    = glm::vec3{ 0, 0, 1 };
    l2.position = glm::vec3{ 8, -6, 11 };
    core::PointLight l3;
    l2.color    = glm::vec3{ 1, 0, 0 };
    l2.position = glm::vec3{ -10, 28, -19 };
    */

    gpuLightPoints.lights[0]  = l0;
    /*
    gpuLightPoints.lights[1]  = l1;
    gpuLightPoints.lights[2]  = l2;
    gpuLightPoints.lights[3]  = l3;
    */
    gpuLightPoints.lightCount = 1;
    m_pointLights->uploadData(core::toBytesPod(gpuLightPoints), core::renderer::STATIC, 0);
}

void TestLayer::onAttach()
{
}

void TestLayer::onDetach()
{
    core::renderer::Renderer::shutdown();
}

void TestLayer::onUpdate(const float delta)
{
    m_camera.onUpdate(delta);
}

void TestLayer::onRender()
{
    core::renderer::SceneDescription sceneDescription;
    sceneDescription.camera      = makeRef<core::Camera>(m_camera);
    sceneDescription.pointLights = m_pointLights;
    core::renderer::Renderer::beginScene(sceneDescription);

    const core::assets::AssetManager& am = core::Application::get().getAssetManager();

    const auto& shader = am.getAsset<core::renderer::Shader>(m_shaderHandle);
    const auto& model  = am.getAsset<core::geometry::Model>(m_modelHandle);

    for (const auto& mesh : model->getMeshes()) {
        const auto& material = mesh.getMaterial();
        if (!material) { continue; }
        glm::mat4 transform = model->getGlobalTransform() * mesh.getLocalTransform();
        core::renderer::Renderer::draw(mesh.getVertexArray(), material, transform, shader);
    }

    core::renderer::Renderer::endScene();
}

void TestLayer::onEvent(core::Event& e)
{
    core::EventHandler resizeHandler(e);
    core::EventHandler inputHandler(e);

    resizeHandler.handle<core::WindowResizeEvent>(
        [this](const core::WindowResizeEvent& windowResize) {
            m_camera.setViewportWidth(windowResize.getWidth());
            m_camera.setViewportHeight(windowResize.getHeight());
            return false; // do not consume resize events
        });

    inputHandler.handle<core::KeyPressEvent>([this](const core::KeyPressEvent& keyPress) {
        auto& am             = core::Application::get().getAssetManager();
        auto model           = am.getAsset<core::geometry::Model>(m_modelHandle);
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
