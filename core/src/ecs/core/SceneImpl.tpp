#pragma once

#include "ecs/core/Scene.hpp"
#include "script/NativeScript.hpp"


namespace siren::core
{
template <typename T>
    requires(std::derived_from<T, NativeScript>)
void Scene::bind(const EntityHandle entity)
{
    auto& scriptComponent = Emplace<ScriptContainerComponent>(entity);
    scriptComponent.scripts.push_back(CreateOwn<T>(this, entity));
}
} // namespace siren::core
