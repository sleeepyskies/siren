#pragma once

#include "ecs/core/World.hpp"
#include "script/native_script.hpp"


namespace siren::core
{
template <typename T>
    requires(std::derived_from<T, NativeScript>)
void World::bind(const EntityHandle entity) {
    auto& scriptComponent = emplace<ScriptContainerComponent>(entity);
    scriptComponent.scripts.push_back(create_own<T>(this, entity));
}
} // namespace siren::core
