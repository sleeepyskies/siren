#pragma once

#include "secsTypes.hpp"

namespace core::secs
{
/**
 * @brief An Entity in sle is just an unsigned 32-bit integer id, as well as a component mask
 * representing which components this entity has.
 */
struct Entity {
public:
    explicit Entity(const EntityID id) : m_id(id){};

    EntityID id() const
    {
        return m_id;
    }
    bool alive() const
    {
        return m_id != 0;
    }
    explicit operator bool() const
    {
        return alive();
    }
    void kill()
    {
        m_id = 0;
    }

    bool test(const size_t pos) const
    {
        return m_mask.test(pos);
    }
    void flip(const size_t pos)
    {
        m_mask.flip(pos);
    }
    bool operator[](const size_t pos) const
    {
        return test(pos);
    }

private:
    EntityID m_id = 0;
    ComponentMask m_mask{};
};
} // namespace core::secs
