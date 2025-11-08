#pragma once

#include "ecs/core/Component.hpp"
#include "script/NativeScript.hpp"


namespace siren::core
{

struct ScriptContainerComponent final : Component
{
    Vector<Own<NativeScript>> scripts{ };
};

} // namespace siren::ecs
