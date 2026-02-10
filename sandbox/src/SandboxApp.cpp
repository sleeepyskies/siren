#include "SandboxApp.hpp"

#include "scripts/PlayerController.hpp"
#include "scripts/ThirdPersonCamera.hpp"

#include "assets/AssetServer.hpp"

#include "core/Time.hpp"

#include "ecs/components/MeshComponent.hpp"
#include "ecs/components/PointLightComponent.hpp"
#include "ecs/components/RenderContextComponent.hpp"
#include "ecs/components/ScriptContainerComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/SceneImpl.tpp"
#include "ecs/core/World.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "ecs/systems/ScriptSystem.hpp"

#include "events/Events.hpp"

#include "renderer/Renderer.hpp"
#include "../../core/src/core/FileSystem.hpp"

#include "window/WindowModule.hpp"


namespace siren::sandbox
{
constexpr i32 max_rand_pos = 2;

void SandboxApp::init() {
    auto& asset_serer = core::Locator<core::AssetServer>::locate();
    auto handle       = asset_serer.load<core::Mesh>(core::AssetPath::parse("hi"));

    // boring scene setup + camera
    {
        const auto dim = core::window().GetSize();
        const auto e   = m_scene.Create();
        m_scene.emplace<core::ScriptContainerComponent>(e);
        m_scene.emplace<core::TransformComponent>(e);
        m_scene.bind<editor::PlayerController>(e);
        m_scene.bind<editor::ThirdPersonCamera>(e);
        m_scene.emplaceSingleton<core::RenderContextComponent>(
            &m_scene.Emplace<core::ThirdPersonCameraComponent>(e, dim.x, dim.y)
        );
    }

    // skybox
    {
        const auto e      = m_scene.Create();
        const auto handle = core::Assets().Import("ass://cubemaps/skybox/sky.cube");
        auto& skylight    = m_scene.Emplace<core::SkyLightComponent>(e, handle);

        auto& rcc           = m_scene.GetSingleton<core::RenderContextComponent>();
        rcc.skyBoxComponent = &skylight;
    }

    // setup environment entity
    {
        auto& am = core::Assets();
        // const auto meshHandle = am.Import("ass://models/gltf/main_sponza/NewSponza_Main_glTF_003.gltf");
        const auto meshHandle = am.Import("ass://models/gltf/car/scene.gltf");
        SIREN_ASSERT(meshHandle, "Invalid mesh");
        const auto e = m_scene.Create();
        m_scene.emplace<core::TransformComponent>(e);
        m_scene.Emplace<core::MeshComponent>(e, meshHandle);
    }

    // add lights
    if constexpr (true) {
        for (i32 i = 0; i < core::MAX_LIGHT_COUNT; i++) {
            const auto e = m_scene.Create();
            glm::vec3 translation{
                0,
                3,
                i * 2,
            };
            glm::vec3 color{ std::rand() % 100 / 100.f, std::rand() % 100 / 100.f, std::rand() % 100 / 100.f };
            m_scene.emplace<core::TransformComponent>(e, translation, glm::vec3{ 0 }, glm::vec3{ 1 });
            m_scene.emplace<core::PointLightComponent>(e, color);
        }
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
                    core::Renderer().reload_shaders();
                }
                return false;
            }
        );
    }
}

void SandboxApp::on_update(const float delta) {
    m_scene.onUpdate(delta);

    static u64 guard = 0;
    guard++;

    if (guard % 180 == 0) {
        const auto title = std::format(
            "Siren (fps: {}) (frame time: {})",
            1 / core::Time::delta(),
            core::Time::delta()
        );
        core::window().SetTitle(title);
    }
}

void SandboxApp::on_render() {
    m_scene.onRender();
}
} // siren
