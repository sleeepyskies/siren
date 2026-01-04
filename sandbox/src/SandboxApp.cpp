#include "SandboxApp.hpp"

#include "scripts/PlayerController.hpp"
#include "scripts/ThirdPersonCamera.hpp"

#include "assets/AssetModule.hpp"

#include "ecs/components/MeshComponent.hpp"
#include "ecs/components/RenderContextComponent.hpp"
#include "ecs/components/ScriptContainerComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "ecs/core/Scene.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "ecs/systems/ScriptSystem.hpp"

#include "events/Events.hpp"

#include "renderer/RenderModule.hpp"
#include "filesystem/FileSystemModule.hpp"

#include "window/WindowModule.hpp"


namespace siren::sandbox
{
void SandboxApp::Init()
{
    s_instance->RegisterModule<core::FileSystemModule>();
    s_instance->RegisterModule<core::AssetModule>();
    s_instance->RegisterModule<core::RenderModule>();

    // boring scene setup + camera
    {
        const auto dim = core::window().getSize();
        const auto e   = m_scene.create();
        m_scene.emplace<core::ScriptContainerComponent>(e);
        m_scene.emplace<core::TransformComponent>(e);
        m_scene.bind<editor::PlayerController>(e);
        m_scene.bind<editor::ThirdPersonCamera>(e);
        m_scene.emplaceSingleton<core::RenderContextComponent>(
            &m_scene.emplace<core::ThirdPersonCameraComponent>(e, dim.x, dim.y)
        );
    }

    // setup environment entity
    {
        auto& am              = core::Assets();
        const auto meshHandle = am.importAsset("ass://models/gltf/main_sponza/NewSponza_Main_glTF_003.gltf");
        // const auto meshHandle = am.importAsset("ass://models/gltf/car/scene.gltf");
        SirenAssert(meshHandle, "Invalid mesh");
        const auto e = m_scene.create();
        m_scene.emplace<core::TransformComponent>(e);
        m_scene.emplace<core::MeshComponent>(e, meshHandle);
    }

    // start systems
    {
        m_scene.start<core::ScriptSystem>(core::LogicPhase);
        m_scene.start<core::RenderSystem>(core::RenderPhase);
    }

    // callbacks
    {
        GetEventBus().Subscribe<core::KeyPressedEvent>(
            [this] (auto& event) {
                if (event.key == core::KeyCode::ESC) {
                    GetEventBus().Emit<core::AppCloseEvent>();
                }
                return false;
            }
        );
        GetEventBus().Subscribe<core::KeyPressedEvent>(
            [] (auto& event) {
                if (event.key == core::KeyCode::F1) {
                    core::Assets().reloadAssetType(core::AssetType::Shader);
                }
                return false;
            }
        );
    }
}

void SandboxApp::OnUpdate(const float delta)
{
    m_scene.onUpdate(delta);
}

void SandboxApp::OnRender()
{
    m_scene.onRender();
}
} // siren
