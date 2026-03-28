module;

#include <entt/entt.hpp>

export module siren.ecs.world;

import siren.ecs.system;
import siren.ecs.signal;
import siren.ecs.event;
import siren.ecs.entity;

namespace siren::ecs {

/**
 * @class World
 * @brief The main storage and API for the siren ecs.
 */
export class World {
public:
    /** @brief Default constructs a new World. */
    World() = default;

    /**
     * @brief Creates a new @ref Entity in the world.
     * @return A handle to a newly created @ref Entity.
     */
    [[nodiscard]] auto create() -> Entity;

    /**
     * @brief Removes and destroys an @ref Entity from the world.
     * @param e The @ref Entity to destroy.
     */
    auto destroy(Entity e) -> void;

    /**
     * @brief Adds a resource to the world.
     * @tparam T The resource type to add.
     * @tparam Args Any args needed to construct the resource T.
     * @param args The parameters used to construct the resource.
     * @return A Resource<T&> wrapper providing access to the resource.
     */
    template <typename T, typename... Args>
    auto add_resource(Args... args) -> Resource<T&> {
        return m_registry.ctx().emplace<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Provides mutable access to some resource of the world.
     * @tparam T The underlying resource type to fetch.
     * @return A Resource<T&> wrapper to access the resource.
     */
    template <typename T>
    [[nodiscard]] auto resource() -> Resource<T&> {
        return Resource<T&>{ m_registry.ctx().get<T>() };
    }

    /**
     * @brief Checks if the world contains the given resource.
     * @tparam T The resource type to check.
     * @return True if the world contains this resource, false otherwise.
     */
    template <typename T>
    [[nodiscard]] auto has_resource() -> bool {
        return m_registry.ctx().contains<T>();
    }

    /**
     * @brief Removes the given resource type from the world.
     * @tparam T The resource type to check.
     * @return True if the resource was removed, false otherwise.
     */
    template <typename T>
    auto remove_resource() -> bool {
        return m_registry.ctx().erase<T>();
    }

    /**
     * @brief Resolves some provided type either into a @ref Query
     * or a @ref Resource.
     * @tparam T The type to resolve.
     * @return Either a @ref Query or a @ref Resource.
     * @note Some special resources such as the @ref SignalBus have shorthands provided,
     * meaning the user may request them in a function as either
     * func(Resource<SignalBus> signal_bus) {...} or func(SignalBus& signal_bus) {...}
     */
    template <typename T>
    [[nodiscard]]
    constexpr auto resolve() -> auto {
        if constexpr (IsQuery_v<T>) {
            using Args = QueryTraits<T>::Args;
            return Query<Args...>{ &m_registry };
        } else if constexpr (IsResource_v<T>) {
            using Inner = ResourceTraits<T>::Inner;
            return resource<Inner>();
        } else if constexpr (IsSignalBus_v<T>) {
            return *resource<SignalBus>();
        } else if constexpr (IsEventBuffer_v<T>) {
            using EventType = EventTraits<T>::EventType;
            return *resource<EventBus>()->event_buffer<EventType>();
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
