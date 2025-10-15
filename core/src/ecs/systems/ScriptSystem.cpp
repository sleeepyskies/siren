#include "ScriptSystem.hpp"

#include "ecs/components/ScriptContainerComponent.hpp"
#include "ecs/core/Scene.hpp"
#include "script/NativeScript.hpp"

namespace siren::ecs
{

void ScriptSystem::onReady(Scene& scene)
{
    for (const auto e : scene.getWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.getSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onReady(); //
        }
    }
}

void ScriptSystem::onShutdown(Scene& scene)
{
    for (const auto e : scene.getWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.getSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onShutdown(); //
        }
    }
}

void ScriptSystem::onUpdate(const float delta, Scene& scene)
{
    for (const auto e : scene.getWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.getSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onUpdate(delta); //
        }
    }
}

} // namespace siren::ecs