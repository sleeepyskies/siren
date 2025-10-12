#pragma once

#include "ecs/core/System.hpp"

namespace siren::ecs
{

class ThirdPersonCameraSystem final : public System
{
public:
    void onUpdate(float delta, Scene& scene) override;
    void onReady(Scene& scene) override;
    void onShutdown(Scene& scene) override;
};

} // namespace siren::ecs
