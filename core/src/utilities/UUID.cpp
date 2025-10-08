#include "UUID.hpp"
#include <random>

namespace siren::utilities
{

UUID::UUID()
{
    static std::mt19937_64 random(std::chrono::system_clock::now().time_since_epoch().count());
    m_uuid = random();
}

UUID::UUID(const uint64_t uuid) : m_uuid(uuid)
{
}

UUID UUID::invalid()
{
    return UUID{ 0 };
}

bool UUID::operator==(const UUID& other) const
{
    return this->m_uuid == other.m_uuid;
}

bool UUID::operator<(const UUID& other) const
{
    return this->m_uuid < other.m_uuid;
}

UUID::operator bool() const
{
    return m_uuid != 0;
}

} // namespace siren::utilities
