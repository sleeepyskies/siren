#pragma once

#include "ecs/core/Component.hpp"

namespace siren::ecs
{

struct TagComponent final : Component {
    std::string tag;

    explicit TagComponent(const std::string& tag) : tag(tag){};
};

} // namespace siren::ecs