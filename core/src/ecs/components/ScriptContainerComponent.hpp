#pragma once

#include "ecs/core/Component.hpp"
#include "script/native_script.hpp"


namespace siren::core
{

struct ScriptContainerComponent final : Component {
    std::vector<std::unique_ptr<NativeScript>> scripts{ };
};

} // namespace siren::ecs
