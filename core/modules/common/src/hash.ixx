export module siren.common:hash;

import :concepts;
import :typedefs;

export
{
template <siren::IsHashable Obj>
struct std::hash<Obj> {
    auto operator()(const Obj& obj) const noexcept -> siren::usize {
        return obj.hash();
    }
};
}

