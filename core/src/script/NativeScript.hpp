#pragma once

#include "ecs/core/EntityManager.hpp"
#include "ecs/core/World.hpp"

#include <ecs/components/TransformComponent.hpp>


namespace siren::core
{
class NativeScript {
public:
    NativeScript(World* scene, const EntityHandle entityHandle)
        : entityHandle(entityHandle), scene(scene) { }

    virtual ~NativeScript() = default;

    virtual void onReady() { }

    virtual void onUpdate(float delta) { }

    virtual void onShutdown() { }

    virtual void onPause() { }

    virtual void onResume() { }

protected:
    template <typename T>
        requires(std::derived_from<T, Component>)
    T& get() const {
        return scene->get<T>(entityHandle);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T* getSafe() const {
        return scene->GetSafe<T>(entityHandle);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T& getSingleton() const {
        return scene->GetSingleton<T>();
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T* getSingletonSafe() const {
        return scene->GetSingletonSafe<T>();
    }

    template <typename T, typename... Args>
        requires(std::derived_from<T, Component>)
    T& emplace(Args&&... args) {
        return scene->emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T& remove() {
        return scene->remove<T>(entityHandle);
    }

private:
    // XXX: should these be private? make just expose getters to inheritors? or a large set of
    // predefined functions? since providing scene directly seems dangerous maybe

    EntityHandle entityHandle = utilities::UUID::invalid();
    World* scene              = nullptr;
};
} // namespace siren::script
