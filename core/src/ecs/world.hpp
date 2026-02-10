#pragma once
#include "entt.hpp"


namespace siren::core
{

class Entity {
public:
    explicit Entity(entt::registry* registry) : m_handle(*registry, registry->create()) { }

private:
    entt::handle m_handle;
};

/// @brief A Query denotes all component types a system needs to run.
template <typename... Components>
class Query {
public:
    explicit Query(entt::registry& registry) : m_registry(registry), m_view(m_registry.view<Components...>()) { }

    auto begin() {
        auto view = m_registry.view<int>();
        view.begin();
        return m_view.begin();
    }

private:
    entt::registry& m_registry;
    entt::view<> m_view;
};

/**
 * @brief The main API for the ecs in siren. Acts as a wrapper around entt,
 * however World provides some useful additional features such as DI and
 * scheduling.
 */
class World {
public:
    World();

    auto

    auto registry() -> entt::registry;
    auto register_system() -> void;
    auto progress() -> bool;

private:
    entt::registry registry;
};
}
