#include "MaterialKey.hpp"

namespace siren::renderer
{
bool MaterialKey::operator==(const MaterialKey& other) const
{
    return std::memcmp(this, &other, sizeof(MaterialKey)) == 0;
}
} // namespace siren::renderer