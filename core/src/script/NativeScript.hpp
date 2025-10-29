#pragma once

#include "ecs/core/EntityManager.hpp"
#include "ecs/core/Scene.hpp"

#include <ecs/components/TransformComponent.hpp>


namespace siren::core
{

class NativeScript
{
public:
    NativeScript(Scene* scene, const EntityHandle entityHandle)
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
    T& get()
    {
        return scene->get<T>(entityHandle);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T* getSafe()
    {
        return scene->getSafe<T>(entityHandle);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T& getSingleton()
    {
        return scene->getSingleton<T>();
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T* getSingletonSafe()
    {
        return scene->getSingletonSafe<T>();
    }

    template <typename T, typename... Args>
        requires(std::derived_from<T, Component>)
    T& emplace(Args&&... args)
    {
        return scene->emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
        requires(std::derived_from<T, Component>)
    T& remove()
    {
        return scene->remove<T>(entityHandle);
    }

private:
    // XXX: should these be private? make just expose getters to inheritors? or a large set of
    // predefined functions? since providing scene directly seems dangerous maybe

    EntityHandle entityHandle = utilities::UUID::invalid();
    Scene* scene              = nullptr;
};

} // namespace siren::script
