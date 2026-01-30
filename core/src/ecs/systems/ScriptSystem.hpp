#pragma once

#include "ecs/core/System.hpp"

namespace siren::core
{

class ScriptSystem final : public System
{
public:
    void onReady(World& scene) override;
    void onShutdown(World& scene) override;
    void onUpdate(float delta, World& scene) override;
    void onPause(World& scene) override;
    void onResume(World& scene) override;
};

} // namespace siren::ecs
