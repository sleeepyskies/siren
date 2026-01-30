#pragma once

namespace siren::core
{

template <typename T>
concept Serializable = requires (const T& t) { t.to_string(); };

template <Serializable Derived>
struct Serialize
{
    const char* serialize() const { return static_cast<const Derived*>(this)->to_string(); }
};

struct Player : Serialize<Player>
{
    const char* to_string() const { return ""; }
};

static inline test() {
    Player p;
    p.serialize();
}

} // namespace siren::core
