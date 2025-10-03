#include "TestLayer.hpp"

#include "core/Application.hpp"
#include "events/WindowEvent.hpp"
#include "geometry/Model.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"

namespace siren
{

void TestLayer::onAttach()
{
    auto& am                   = core::Application::get().getAssetManager();
    const fs::path& workingDir = core::Application::get().getProperties().workingDirectory;
    const fs::path assetDir    = workingDir / "assets";

    // load shaders and model
    const fs::path shaderPath = assetDir / "shaders" / "basic.sshg";
    const fs::path modelPath  = assetDir / "models" / "gltf" / "stick_man" / "scene.gltf";
    const core::Maybe<core::assets::AssetHandle> shaderRes = am.importAsset(shaderPath);
    const core::Maybe<core::assets::AssetHandle> modelRes  = am.importAsset(modelPath);
    if (!shaderRes || !modelRes) err("Could not load scene or model");
    m_shaderHandle = *shaderRes;
    m_modelHandle  = *modelRes;
}

void TestLayer::onDetach()
{
    core::renderer::Renderer::shutdown();
}

void TestLayer::onUpdate(const float delta)
{
}

void TestLayer::onRender()
{
    core::renderer::Renderer::beginScene();

    const core::assets::AssetManager& am = core::Application::get().getAssetManager();

    const auto& shader = am.getAsset<core::renderer::Shader>(m_shaderHandle);
    const auto& model  = am.getAsset<core::geometry::Model>(m_modelHandle);

    for (const auto& mesh : model->getMeshes()) {
        const auto& material = mesh.getMaterial();
        if (!material) continue;
        glm::mat4 transform = model->getGlobalTransform() * mesh.getLocalTransform();
        core::renderer::Renderer::submit(mesh.getVertexArray(), material, transform);
    }

    core::renderer::Renderer::endScene();
}

void TestLayer::onEvent(core::Event& e)
{
    core::EventHandler handler(e);
    handler.handle<core::WindowResizeEvent>([this](core::WindowResizeEvent& e) {
        return false; // do not consume resize events
    });
}

} // namespace siren
