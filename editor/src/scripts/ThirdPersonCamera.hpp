#pragma once

#include "script/NativeScript.hpp"


namespace siren::editor
{

class ThirdPersonCamera final : public core::NativeScript
{
public:
    ThirdPersonCamera(core::Scene* scene, const core::EntityHandle entityHandle)
        : NativeScript(scene, entityHandle) { };

    void onUpdate(float delta) override;
    void onReady() override;
    void onShutdown() override;
    void onPause() override;
    void onResume() override;
};

} // namespace siren
