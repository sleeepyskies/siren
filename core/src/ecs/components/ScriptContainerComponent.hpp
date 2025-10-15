#pragma once

#include "ecs/core/Component.hpp"
#include "script/NativeScript.hpp"

namespace siren::ecs
{

struct ScriptContainerComponent final : Component {
    std::vector<Uref<script::NativeScript>> scripts{};
};

} // namespace siren::ecs
