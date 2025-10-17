#pragma once

#include "script/NativeScript.hpp"

namespace siren
{

class ThirdPersonCamera final : public script::NativeScript
{
public:
    ThirdPersonCamera(ecs::Scene* scene, const ecs::EntityHandle entityHandle)
        : NativeScript(scene, entityHandle){};

    void onUpdate(float delta) override;
    void onReady() override;
    void onShutdown() override;
    void onPause() override;
    void onResume() override;
};

} // namespace siren
