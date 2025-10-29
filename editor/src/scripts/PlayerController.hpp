#pragma once

#include "script/NativeScript.hpp"


namespace siren::editor
{

class PlayerController final : public core::NativeScript
{
public:
    PlayerController(core::Scene* scene, const core::EntityHandle entityHandle)
        : NativeScript(scene, entityHandle) { }

    void onUpdate(float delta) override;

private:
    float m_movementSpeed = 3.f;
};

} // namespace siren
