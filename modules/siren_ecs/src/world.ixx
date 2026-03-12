module;

#include <entt/entt.hpp>

export module siren.ecs.world;

import siren.ecs.traits;
import siren.ecs.system_params;

namespace siren::ecs {

export using Entity = entt::handle;

export class World {
public:
    World() = default;

    auto create() -> Entity;
    auto destroy(Entity e) -> void;

    template <typename Resource, typename... Args>
    auto add_resource(Args... args) -> Resource& {
        return m_registry.ctx().emplace<Resource>(std::forward<Args>(args)...);
    }

    template <typename Resource>
    auto resource() -> Resource& {
        return m_registry.ctx().get<Resource>();
    }

    template <typename Resource>
    auto has_resource() -> bool {
        return m_registry.ctx().contains<Resource>();
    }

    template <typename Resource>
    auto remove_resource() -> bool {
        return m_registry.ctx().erase<Resource>();
    }

    template <typename T>
    constexpr auto resolve() -> auto {
        if constexpr (IsQuery_v<T>) {
            using Args = QueryTraits<T>::Args;
            return Query<Args...>{ &m_registry };
        } else if constexpr (IsResource_v<T>) {
            using Inner = ResourceTraits<T>::Inner;
            return resource<Inner>();
        } else {
            static_assert(false, "Invalid argument passed to World::resolve<>()");
        }
    }

private:
    entt::registry m_registry;
};

auto World::create() -> Entity {
    return entt::handle{ m_registry, m_registry.create() };
}

auto World::destroy(const Entity e) -> void {
    m_registry.destroy(e.entity());
}
} // namespace siren::ecs
