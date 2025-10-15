#pragma once

#include "ecs/core/Scene.hpp"
#include "script/NativeScript.hpp"

namespace siren::ecs
{

template <typename T>
    requires(std::derived_from<T, script::NativeScript>)
void Scene::bind(const EntityHandle entity)
{
    auto& scriptComponent = emplace<ScriptContainerComponent>(entity);
    scriptComponent.scripts.push_back(makeUref<T>(this, entity));
}

} // namespace siren::ecs
