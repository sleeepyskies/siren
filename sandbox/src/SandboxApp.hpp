#pragma once
#include "core/App.hpp"

#include "ecs/components/ThirdPersonCameraComponent.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::sandbox
{
class SandboxApp final : public core::App
{
public:
    void initialize() override;
    void onUpdate(float delta) override;
    void onRender() override;

private:
    friend class App;

    explicit SandboxApp(const Properties& properties) : App(properties) { }

    core::Scene m_scene{ };
    core::ThirdPersonCameraComponent* m_camera = nullptr;
};
} // siren::sandbox
