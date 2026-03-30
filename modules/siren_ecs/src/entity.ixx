module;

#include <entt/entt.hpp>

export module siren.ecs.entity;

namespace siren::ecs {

/// @todo Proper wrapper for entity class, make fit other engine more with Entity::invalid() etc.

class EntityDemo {
public:
    [[noreturn]] static auto invalid() noexcept -> EntityDemo;
    [[noreturn]] auto is_invalid() const noexcept -> bool;

private:
};

export using Entity = entt::handle;

export inline constexpr entt::null_t NullEntity{ };

} // namespace siren::ecs
