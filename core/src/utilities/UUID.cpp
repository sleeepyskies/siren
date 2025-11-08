#include "UUID.hpp"
#include <random>


namespace siren::utilities
{
static std::mt19937_64 random(std::chrono::system_clock::now().time_since_epoch().count());

void UUID::setSeed(const u64 seed)
{
    random.seed(seed);
}

UUID UUID::create()
{
    return UUID{ random() };
}

UUID UUID::invalid()
{
    return UUID{ };
}

void UUID::invalidate()
{
    m_uuid = 0;
}

u64 UUID::id() const
{
    return m_uuid;
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

UUID::operator u64() const
{
    return m_uuid;
}

UUID::UUID(const u64 uuid) : m_uuid(uuid) { }

} // namespace siren::utilities
