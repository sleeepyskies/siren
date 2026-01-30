#pragma once
#include "core/App.hpp"

#include "ecs/components/ThirdPersonCameraComponent.hpp"
#include "ecs/core/World.hpp"


namespace siren::sandbox
{
class SandboxApp final : public core::App
{
public:
    void init() override;
    void on_update(float delta) override;
    void on_render() override;

private:
    friend class App;

    explicit SandboxApp(const Properties& properties) : App(properties) { }

    core::World m_scene{ };
    core::ThirdPersonCameraComponent* m_camera = nullptr;
};
} // siren::sandbox
