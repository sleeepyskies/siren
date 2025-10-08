#pragma once

#include "ComponentBitMap.hpp"
#include "ECSProperties.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{

/// @brief A UUID representing an entity
using EntityHandle  = utilities::UUID;
/// @brief A bitmask used to indicate what components an entity has assigned.
using ComponentMask = std::bitset<MAX_COMPONENTS>;

/**
 * @brief Responsible for the creation, destruction and invalidation of EntityHandle's, as well as
 * managing the ComponentMask of each entity.
 */
class EntityManager
{
public:
    /// @brief Creates a new entity.
    EntityHandle createEntity();

    /// @brief Invalidates the entity and erases its mask.
    bool destroyEntity(EntityHandle entity);

    /// @brief Updates the given entities bitmask to correspond with its new component type.
    /// @returns true on success, false otherwise.
    template <typename T>
    bool addComponent(const EntityHandle entity)
    {
        if (!entity) {
            wrn("Attempting to assign a component to an invalid entity");
            return false;
        }

        if (!m_entityToMask.contains(entity)) {
            wrn("Attempting to assign a component to an entity with no registered mask");
            return false;
        }

        m_entityToMask[entity].set(ComponentBitMap::getBitIndex<T>());

        return true;
    }

    /**
     * @brief Removes the given entities bitmask corresponding with the component type.
     * @returns true on success, false otherwise.
     */
    template <typename T>
    bool removeComponent(EntityHandle& entity)
    {
        if (!entity) {
            wrn("Attempting to remove a component from an invalid entity");
            return false;
        }

        if (!m_entityToMask.contains(entity)) {
            wrn("Attempting to assign a component to an entity with no registered mask");
            return false;
        }

        m_entityToMask[entity].set(ComponentBitMap::getBitIndex<T>());

        return true;
    }

private:
    HashMap<EntityHandle, ComponentMask> m_entityToMask{};
};

} // namespace siren::ecs
