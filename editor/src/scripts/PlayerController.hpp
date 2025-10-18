#pragma once

#include "script/NativeScript.hpp"

namespace siren
{

class PlayerController final : public script::NativeScript
{
public:
    PlayerController(ecs::Scene* scene, const ecs::EntityHandle entityHandle)
        : NativeScript(scene, entityHandle)
    {
    }

    void onUpdate(float delta) override;

private:
    float m_movementSpeed = 3.f;
};

} // namespace siren
