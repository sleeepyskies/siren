#pragma once

#include "ComponentBitMap.hpp"
#include "ECSProperties.hpp"
#include "EntityHandle.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

/**
 * @brief Responsible for the creation, destruction and invalidation of EntityHandle's, as well as
 * managing the ComponentMask of each entity.
 */
class EntityManager
{
public:
    /// @brief A bitmask used to indicate what components an entity has assigned.
    using ComponentMask = std::bitset<MAX_COMPONENTS>;

    /// @brief Creates a new entity.
    EntityHandle create();

    /// @brief Invalidates the entity and erases its mask.
    void destroy(EntityHandle entity);

    /// @brief Returns all entities that have the component bits set in the mask
    std::vector<EntityHandle> getWith(ComponentMask components) const;

    /// @brief Returns all entities
    std::vector<EntityHandle> getAll() const;

    /// @brief Updates the given entities bitmask to correspond with its new component type.
    template <typename T>
    void add(const EntityHandle entity)
    {
        if (!entity) { return; }
        if (!m_entityToMask.contains(entity)) { return; }

        m_entityToMask[entity].set(ComponentBitMap::getBitIndex<T>());
    }

    /// @brief Removes the given entities bitmask corresponding with the component type.
    template <typename T>
    void remove(EntityHandle& entity)
    {
        if (!entity) { return; }
        if (!m_entityToMask.contains(entity)) { return; }

        m_entityToMask[entity].reset(ComponentBitMap::getBitIndex<T>());
    }

private:
    HashMap<EntityHandle, ComponentMask> m_entityToMask{};
    HashMap<EntityHandle, size_t> m_entityToIndex{};
    std::vector<EntityHandle> m_alive{};
};

} // namespace siren::ecs
