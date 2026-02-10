#pragma once

#include "ecs/core/Component.hpp"
#include "script/NativeScript.hpp"


namespace siren::core
{

struct ScriptContainerComponent final : Component {
    std::vector<std::unique_ptr<NativeScript>> scripts{ };
};

} // namespace siren::ecs
