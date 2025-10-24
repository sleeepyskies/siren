#include "MaterialKey.hpp"

namespace siren::core
{
bool MaterialKey::operator==(const MaterialKey& other) const
{
    return std::memcmp(this, &other, sizeof(MaterialKey)) == 0;
}
} // namespace siren::core