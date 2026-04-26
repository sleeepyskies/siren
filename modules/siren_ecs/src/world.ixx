module;

#include <new>
#include <string>
#include <libassert/assert.hpp>
#include <entt/entt.hpp>
#include <utility>
#include <type_traits>
#include <any>

export module siren.ecs:world;

import :system;
import :signals;
import :event;
import :entity;

namespace siren::ecs {

/// @todo:
///     bc of using modules, using header only libs like entt suchs, so we use pimpl idiom.
///     this also isnt optimal so best would b to write own ecs but this is for future.

struct ResourceBase {
    virtual ~ResourceBase() = default;
};

template <typename T>
struct ResourceContainer final : ResourceBase {
    T value;

    template <typename... Args>
    explicit ResourceContainer(Args&&... args) : value(std::forward<Args>(args)...) { }
};

// ============================================================================
// == MARK: Resource Storage
// ============================================================================
class ResourceStorage {
public:
    using Id_t     = HashedString::HashType;
    using Resource = std::unique_ptr<ResourceBase>;

    template <typename T, typename... Args>
        requires(std::is_constructible_v<T, Args...>)
    auto emplace(Args&&... args) -> T& {
        auto res = std::unique_ptr<ResourceBase>(
            std::make_unique<ResourceContainer<T>>(std::forward<Args>(args)...)
        );
        const auto key      = typehash_of<T>();
        auto [it, inserted] = m_storage.try_emplace(key, std::move(res));
        return static_cast<ResourceContainer<T>*>(it->second.get())->value;
    }

    template <typename T>
    auto get() -> T& {
        auto* instance = find<T>();
        ASSERT(instance != nullptr, "Attempted to call ResourceStorage::get() on non inserted type.");
        return *instance;
    }

    template <typename T>
    auto get() const -> const T& {
        auto* instance = find<T>();
        ASSERT(instance != nullptr, "Attempted to call ResourceStorage::get() on non inserted type.");
        return *instance;
    }

    template <typename T>
    auto find() -> T* {
        auto it = m_storage.find(typehash_of<T>());
        if (it == m_storage.end()) {
            return nullptr;
        }
        auto* container = static_cast<ResourceContainer<T>*>(it->second.get());
        return &container->value;
    }

    template <typename T>
    auto find() const -> const T* {
        auto it = m_storage.find(typehash_of<T>());
        if (it == m_storage.end()) {
            return nullptr;
        }

        auto* container = static_cast<const ResourceContainer<T>*>(it->second.get());
        return &container->value;
    }

    template <typename T>
    auto erase() -> bool { return m_storage.erase(typehash_of<T>()) != 0; }

    template <typename T>
    auto contains() const -> bool { return m_storage.contains(typehash_of<T>()); }

private:
    std::unordered_map<Id_t, Resource> m_storage;
};

// ============================================================================
// == MARK: Forward Declarations
// ============================================================================

export class World;

export class Resolver;

export template <IsReference... Args>
class Query;

export template <typename TSignal>
class SignalBuilder;

// ============================================================================
// == MARK: Query
// ============================================================================

/**
 * @brief An object enabling iteration over a set of entities
 * who have the provided set of components.
 * @tparam Args The component types to query for.
 * @todo Flesh this API out more, cannot direct expose entt due to module issues,
 * so should improve this class.
 */
export template <IsReference... Args>
class Query {
    friend class World;

public:
    explicit Query(const World& world) : m_world(world) { }

    /**
     * @brief Calls @p func on each entity in the Query.
     * The callback receives the entity handle and each requested component via reference.
     */
    template <IsCallable Func>
    auto each(Func&& func) -> void;

private:
    const World& m_world;
};

template <typename T>
struct IsQuery : std::false_type { };

template <typename... Args>
struct IsQuery<Query<Args...>> : std::true_type { };

template <typename T>
inline constexpr bool IsQuery_v = IsQuery<T>::value;

template <typename T>
struct QueryTraits { };

template <typename... Args>
struct QueryTraits<Query<Args...>> {
    using ArgsTuple                  = std::tuple<Args...>;
    static constexpr usize ArgsCount = sizeof...(Args);
};

// ============================================================================
// == MARK: Resolver
// ============================================================================

class Resolver {
public:
    explicit Resolver(World& world) : m_world(world) { }

    template <typename T>
    [[nodiscard]]
    auto resolve() const -> decltype(auto);

private:
    template <typename Tuple>
    struct QueryFromTuple;

    template <typename... Args>
    struct QueryFromTuple<std::tuple<Args...>> {
        static auto make(const World& world) -> Query<Args...>;
    };

    World& m_world;
};

// ============================================================================
// == MARK: SignalBuilder
// ============================================================================
/**
 * @class SignalBuilder
 * @brief Handles constructing signal callbacks.
 * @tparam TSignal The listened to signal.
 */
export template <typename TSignal>
class SignalBuilder {
public:
    SignalBuilder(SignalBus& signal_bus, Resolver& resolver) : m_signal_bus(signal_bus), m_resolver(resolver) { }

    /** @brief Listens to signals only emitted for the provided entity instead of globally. */
    [[nodiscard]]
    auto target(const Entity& entity) -> SignalBuilder& {
        m_target = entity;
        return *this;
    }

    /** @brief Binds the callback and registers the signal listener. */
    template <typename Callback>
    auto run(Callback&& callback) -> void {
        using Traits   = FunctionTraits<std::decay_t<Callback>>;
        using ArgsPack = Traits::ArgsPack;

        SignalBus::Callback erased = [resolver = m_resolver, cb = std::forward<Callback>(callback)] (
            void* signal_ptr
        ) mutable {
            TSignal& signal = *static_cast<TSignal*>(signal_ptr);
            // immediately invoke lambda bc we need to unpack the typepack
            [&]<typename... P> (TypePack<P...>) {
                cb(resolve_sig<P>(resolver, signal)...);
            }(ArgsPack{ });
        };

        m_signal_bus.on(typehash_of<TSignal>(), m_target, std::move(erased));
    }

private:
    template <typename T>
    static auto resolve_sig(Resolver& resolver, TSignal& signal) -> decltype(auto) {
        if constexpr (std::is_same_v<std::decay_t<T>, TSignal>) {
            return static_cast<T>(signal);
        }
        return resolver.resolve<T>();
    }

    SignalBus& m_signal_bus;
    Resolver& m_resolver;
    Entity m_target = NullEntity;
};

// ============================================================================
// == MARK: Signals
// ============================================================================
/**
 * @class Signals
 * @brief
 */
export class Signals {
public:
    Signals(SignalBus& signal_bus, const Resolver& resolver) : m_signal_bus(signal_bus), m_resolver(resolver) { }

    /** @brief Begins registering a new signal listener for TSignal. */
    template <typename TSignal>
    auto on() -> SignalBuilder<TSignal> {
        return SignalBuilder<TSignal>(m_signal_bus, m_resolver);
    }

    /** @brief Removes handlers for TSignal on a given entity, or globally. */
    template <typename TSignal>
    auto off(const Entity& target = NullEntity) -> void {
        m_signal_bus.off(typehash_of<TSignal>(), target);
    }

    /** @brief Emits TSignal to all globally subscribed handlers. */
    template <typename TSignal, typename... Args>
    auto emit(Args&&... args) -> void {
        m_signal_bus.emit<TSignal>(std::forward<Args>(args)...);
    }

    /** @brief Emits TSignal targeting a specific entity. */
    template <typename TSignal, typename... Args>
    auto emit_to(const Entity& target, Args&&... args) -> void {
        m_signal_bus.emit_to<TSignal>(target, std::forward<Args>(args)...);
    }

private:
    SignalBus& m_signal_bus;
    Resolver m_resolver;
};

// ============================================================================
// == MARK: World
// ============================================================================

/**
 * @class World
 * @brief The main storage and API for the siren ecs.
 */
class World {
    template <IsReference... Args>
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
     * @return A Resource<T> wrapper providing access to the resource.
     */
    template <typename T, typename... Args>
    auto add_resource(Args&&... args) -> Resource<T> {
        using Type     = std::remove_cvref_t<T>;
        Type& instance = m_resources.emplace<Type>(std::forward<Args>(args)...);
        return Resource<T>{ instance };
    }

    /**
     * @brief Provides mutable access to some resource of the world.
     * @tparam T The underlying resource type to fetch.
     * @return A Resource<T> wrapper to access the resource.
     */
    template <typename T>
    [[nodiscard]] auto resource() -> Resource<T> {
        using Type     = std::remove_cvref_t<T>;
        Type& instance = m_resources.get<T>();
        return Resource<T>{ instance };
    }

    /**
     * @brief Checks if the world contains the given resource.
     * @tparam T The resource type to check.
     * @return True if the world contains this resource, false otherwise.
     */
    template <typename T>
    [[nodiscard]] auto has_resource() -> bool { return m_resources.contains<T>(); }

    /**
     * @brief Removes the given resource type from the world.
     * @tparam T The resource type to check.
     * @return True if the resource was removed, false otherwise.
     */
    template <typename T>
    auto remove_resource() -> bool { return m_resources.erase<T>(); }

    /**
     * @brief Returns a @ref Query object that can iterate over all entities with the provided types.
     * @tparam Args The component types to query.
     * @return A @ref Query of all the provided types.
     */
    template <typename... Args>
    auto query() -> Query<Args...> { return Query<Args...>(*this); }

private:
    entt::registry m_registry;
    ResourceStorage m_resources;
};

template <IsReference ... Args>
template <IsCallable Func>
auto Query<Args...>::each(Func&& func) -> void {
    m_world.m_registry.view<Args...>().each(std::forward<Func>(func));
}

template <typename T>
auto Resolver::resolve() const -> decltype(auto) {
    if constexpr (IsQuery_v<T>) {
        using Tuple = QueryTraits<T>::ArgsTuple;
        return QueryFromTuple<Tuple>::make(m_world);
    } else if constexpr (IsResource_v<T>) {
        using Inner = ResourceTraits<T>::Inner;
        return m_world.resource<Inner>();
    } else if constexpr (IsSignals_v<T>) {
        return Signals{ *m_world.resource<SignalBus>(), *this };
    } else if constexpr (IsEventBuffer_v<T>) {
        using EventType = EventTraits<T>::EventType;
        return *m_world.resource<EventBus>()->event_buffer<EventType>();
    } else {
        static_assert(false, "Invalid argument passed to World::resolve<>()");
    }
}

template <typename... Args>
auto Resolver::QueryFromTuple<std::tuple<Args...>>::make(const World& world) -> Query<Args...> {
    return world.query<Args...>();
}

auto World::create() -> Entity {
    return Entity{ };
}

auto World::invalidate(Entity e) const -> void {
    e.invalidate();
}
} // namespace siren::ecs
