#pragma once

#include "components/MeshComponent.hpp"
#include "core/spch.hpp"


namespace siren::core
{

/// @brief Ensures some type T is a reference.
template <typename T>
concept IsReference = std::is_reference_v<T>;

template <IsReference... Args>
class Query {
    using ViewType = entt::view<entt::type_list<Args...>>;

public:
    explicit Query(const entt::registry* registry) : m_registry(registry) { }

    auto begin() { return m_registry->view<Args...>().begin(); }
    auto end() { return m_registry->view<Args...>().end(); }
    entt::basic_view<int, int, int> view;
    auto x() {
        m_registry->storage()
        entt::meta_getter<>(view)
    }

private:
    const entt::registry* m_registry;
};

inline entt::registry registry;
inline Query<const MeshComponent> query{ registry };

} // namespace siren::core

struct Render {
    auto operator(Query<const Mesh&, const Position&> query)() -> void {
        for (const auto& [mesh, position] : query) {
            ...
        }
    }
};
