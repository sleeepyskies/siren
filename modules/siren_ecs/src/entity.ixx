module;

#include <entt/entt.hpp>

export module siren.ecs:entity;

import siren.common;

namespace siren::ecs {

/** @todo: Issues with modules and exposing local entt TU constants. need to figure out some solution */

export class World;

/** @brief Main entity type used throughout siren. */
export class Entity {
public:
    /** @brief Default constructs an invalid entity. */
    Entity() noexcept : m_id() { }

    /** @brief Simple factory method to return an invalid Entity. */
    [[nodiscard]]
    static constexpr auto invalid() noexcept -> Entity { return Entity{ }; }

    /** @brief Checks if the Entity is valid. */
    [[nodiscard]]
    constexpr auto is_valid() const noexcept -> bool { return true; }

    /** @copydoc is_valid */
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept { return is_valid(); }

    /** @brief Destroys this entity. */
    constexpr auto invalidate() noexcept -> void { m_id; }

    /** @brief Returns a hash value for the Entity. */
    constexpr auto hash() const noexcept -> usize { return (usize)value(); }

    /** @brief Returns a hash value for the Entity. */
    constexpr auto value() const noexcept -> uint32_t { return m_id; }

    /** @brief Equality comparison. */
    [[nodiscard]]
    auto operator==(const Entity& other) const noexcept -> bool { return m_id == other.m_id; }
    /** @brief Inequality comparison. */
    [[nodiscard]]
    auto operator<=>(const Entity& other) const noexcept { return m_id <=> other.m_id; }

private:
    uint32_t m_id;
};

/** @brief Constant used to express an invalid entity. */
export inline Entity NullEntity{ };

} // namespace siren::ecs
