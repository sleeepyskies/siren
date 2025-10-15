#pragma once

#include "ecs/core/System.hpp"

namespace siren::ecs
{

class ScriptSystem final : public System
{
public:
    void onReady(Scene& scene) override;
    void onShutdown(Scene& scene) override;
    void onUpdate(float delta, Scene& scene) override;
};

} // namespace siren::ecs
