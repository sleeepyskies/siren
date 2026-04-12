module;

#include <entt/entt.hpp>
#include <utility>

export module siren.ecs:world;

import :system;
import :signal;
import :event;
import :entity;

namespace siren::ecs {

export class World;

/**
 * @brief An object enabling iteration over a set of entities
 * who have the provided set of components.
 * @tparam Args The component types to query for.
 * @todo Flesh this API out more, cannot direct expose entt due to module issues,
 * so should improve this class.
 */
export template <IsReference... Args>
class Query {
public:
    /** @brief Constructs a new Query. */
    explicit Query(const World& world);

    /**
     * @brief Calls the given function on each entity in the Query.
     * @param func The function to call for each entity in the Query.
     */
    template <IsCallable Func>
    auto each(Func&& func) -> void;

private:
    /** @brief Reference to the @ref World the query originates from. */
    const World& m_world;
};

/**
 * @brief Type trait to check if a type is a @ref Query.
 * @tparam T The type to check.
 */
template <typename T>
struct IsQuery : std::false_type { };

/**
 * @brief Specialization for @ref Query types.
 */
template <typename... Args>
struct IsQuery<Query<Args...>> : std::true_type { };

/**
 * @brief Helper constant for @ref IsQuery.
 * @tparam T The type to check.
 */
template <typename T>
inline constexpr bool IsQuery_v = IsQuery<T>::value;

/**
 * @brief Traits helper to extract information from a @ref Query type.
 * @tparam T The type to extract from.
 */
template <typename T>
struct QueryTraits { };

/**
 * @brief Specialization of @ref QueryTraits for @ref Query.
 */
template <typename... Args>
struct QueryTraits<Query<Args...>> {
    /** @brief The components requested by the query as a std::tuple. */
    using ArgsTuple = std::tuple<Args...>;
    /** @brief The number of components in the query. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

/**
 * @class World
 * @brief The main storage and API for the siren ecs.
 */
export class World {
    template <typename... Args>
    friend class Query;

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
    auto invalidate(Entity e) const -> void;

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
     * @brief Returns a @ref Query object that can iterate over all entities with the provided types.
     * @tparam Args The component types to query.
     * @return A @ref Query of all the provided types.
     */
    template <typename... Args>
    auto query() -> Query<Args...> {
        return Query<Args...>(*this);
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
            return query<Args...>();
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

template <IsReference ... Args>
Query<Args...>::Query(const World& world) : m_world(world) { }

template <IsReference ... Args>
template <IsCallable Func>
auto Query<Args...>::each(Func&& func) -> void {
    m_world.m_registry.view<Args...>().each(std::forward<Func>(func));
}

auto World::create() -> Entity {
    return Entity{ m_registry };
}

auto World::invalidate(Entity e) const -> void {
    e.invalidate();
}
} // namespace siren::ecs
