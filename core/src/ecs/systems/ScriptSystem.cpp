#include "ScriptSystem.hpp"

#include "ecs/components/ScriptContainerComponent.hpp"
#include "ecs/core/World.hpp"
#include "script/NativeScript.hpp"


namespace siren::core
{
void ScriptSystem::onReady(World& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onReady(); //
        }
    }
}

void ScriptSystem::onShutdown(World& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onShutdown(); //
        }
    }
}

void ScriptSystem::onUpdate(const float delta, World& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onUpdate(delta); //
        }
    }
}

void ScriptSystem::onPause(World& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onPause(); //
        }
    }
}

void ScriptSystem::onResume(World& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onResume(); //
        }
    }
}
} // namespace siren::ecs
