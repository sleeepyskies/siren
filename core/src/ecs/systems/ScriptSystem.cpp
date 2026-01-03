#include "ScriptSystem.hpp"

#include "ecs/components/ScriptContainerComponent.hpp"
#include "ecs/core/Scene.hpp"
#include "script/NativeScript.hpp"


namespace siren::core
{
void ScriptSystem::onReady(Scene& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onReady(); //
        }
    }
}

void ScriptSystem::onShutdown(Scene& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onShutdown(); //
        }
    }
}

void ScriptSystem::onUpdate(const float delta, Scene& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onUpdate(delta); //
        }
    }
}

void ScriptSystem::onPause(Scene& scene)
{
    for (const auto e : scene.GetWith<ScriptContainerComponent>()) {
        const auto* scripts = scene.GetSafe<ScriptContainerComponent>(e);
        if (!scripts) { return; }
        for (const auto& script : scripts->scripts) {
            script->onPause(); //
        }
    }
}

void ScriptSystem::onResume(Scene& scene)
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
