module;

#include <entt/entt.hpp>

export module siren.ecs:entity;

import siren.common;

namespace siren::ecs {

/** @brief Main entity type used throughout siren. */
export class Entity {
public:
    /** @brief Default constructs an invalid entity. */
    Entity() noexcept : m_handle() { }
    /** @brief Constructs a new Entity given a registry. */
    explicit Entity(entt::registry& registry) : m_handle(registry, registry.create()) { }

    /** @brief Simple factory method to return an invalid Entity. */
    [[nodiscard]]
    static constexpr auto invalid() noexcept -> Entity { return Entity{ }; }

    /** @brief Checks if the Entity is valid. */
    [[nodiscard]]
    constexpr auto is_valid() const noexcept -> bool { return m_handle.valid(); }

    /** @copydoc is_valid */
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept { return is_valid(); }

    /** @brief Destroys this entity. */
    constexpr auto invalidate() noexcept -> void { m_handle.destroy(); }

    /** @brief Returns a hash value for the Entity. */
    constexpr auto hash() const noexcept -> usize { return (usize)value(); }

    /** @brief Returns a hash value for the Entity. */
    constexpr auto value() const noexcept -> entt::entity { return m_handle.entity(); }

    /** @brief Equality comparison. */
    [[nodiscard]]
    auto operator==(const Entity& other) const noexcept -> bool { return m_handle == other.m_handle; }
    /** @brief Inequality comparison. */
    [[nodiscard]]
    auto operator<=>(const Entity& other) const noexcept { return m_handle <=> other.m_handle; }

private:
    entt::handle m_handle;
};

/** @brief Constant used to express an invalid entity. */
export inline Entity NullEntity{ };

} // namespace siren::ecs
