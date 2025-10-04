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

    // setup hardcoded uniform buffer with hardcoded lights
    GPULightPoints gpuLightPoints{};
    std::vector<core::PointLight> pointLights{};
    core::PointLight l0;
    l0.cx = 1;
    l0.cy = 1;
    l0.cz = 1;
    l0.px = 10;
    l0.py = 42;
    l0.pz = -33;
    core::PointLight l1;
    l1.cx = 0.8;
    l1.cy = 0.5;
    l1.cz = 0.5;
    l1.px = 9;
    l1.py = 13;
    l1.pz = -7;

    gpuLightPoints.lights[0]  = l0;
    gpuLightPoints.lights[1]  = l1;
    gpuLightPoints.lightCount = 2;
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
        if (keyPress.getKeycode() == core::KeyCode::F1) {
            glm::vec3 pos = m_camera.position();
            nfo("Current coordinates: ({} ,{}, {})", pos.x, pos.y, pos.z);
            core::Application::get().getWindow().setVsync(false);
        } else if (keyPress.getKeycode() == core::KeyCode::F2) {
            // reload shaders
            auto& am = core::Application::get().getAssetManager();
            if (!am.reloadAsset(m_shaderHandle)) { err("Could not reload shaders"); }
        }
        return true;
    });
}

} // namespace siren
