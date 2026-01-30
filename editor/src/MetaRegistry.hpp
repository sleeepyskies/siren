#pragma once
#include "entt.hpp"

#include "ecs/core/Component.hpp"
#include "ecs/core/World.hpp"

// todo: better init system

namespace siren::editor
{
class MetaRegistry
{
public:
    using HasComponentFn = bool(*)(const core::World&, core::EntityHandle);
    using GetComponentFn = entt::meta_handle(*)(core::World&, core::EntityHandle);

    struct ComponentInfo
    {
        entt::meta_type t;
        HasComponentFn has;
        GetComponentFn get;
    };

    static MetaRegistry& get()
    {
        static MetaRegistry i{ };
        return i;
    }

    template <typename T>
        requires (std::derived_from<T, core::Component>)
    entt::meta_factory<T> reflect(const char* name)
    {
        const auto factory = entt::meta_factory<T>()
                             .type(name)
                             .template base<core::Component>();

        const entt::meta_type t = entt::resolve<T>();

        m_components.emplace_back(
            t,
            [] (const core::World& scene, const core::EntityHandle entity) {
                return scene.hasComponent<T>(entity);
            },
            [] (core::World& scene, const core::EntityHandle entity) {
                T& comp = scene.get<T>(entity);
                return entt::meta_handle{ comp };
            }
        );
        m_idToIndex[t.id()] = m_components.size() - 1;

        return factory;
    }

    template <typename T>
        requires (!std::derived_from<T, core::Component>)
    entt::meta_factory<T> reflect(const char* name)
    {
        return entt::meta_factory<T>()
                .type(name);
    }

    bool hasComponent(const entt::meta_type& t, const core::World& scene, const core::EntityHandle& entity)
    {
        return m_components[m_idToIndex[t.id()]].has(scene, entity);
    }

    entt::meta_handle getComponent(const entt::meta_type& t, core::World& scene, const core::EntityHandle& entity)
    {
        return m_components[m_idToIndex[t.id()]].get(scene, entity);
    }

    const Vector<ComponentInfo>& getComponents() const { return m_components; }

private:
    MetaRegistry();

    void registerPrimitives();
    void registerTypes();
    void registerComponents();

    Vector<ComponentInfo> m_components{ };
    HashMap<entt::id_type, size_t> m_idToIndex{ };
};
} // siren::editor
