#pragma once

#include "script/native_script.hpp"


namespace siren::editor
{

class PlayerController final : public core::NativeScript
{
public:
    PlayerController(core::World* scene, const core::EntityHandle entityHandle)
        : NativeScript(scene, entityHandle) { }

    void onUpdate(float delta) override;

private:
    float m_movementSpeed = 3.f;
};

} // namespace siren
