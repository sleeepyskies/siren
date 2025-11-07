#pragma once

#include "ecs/core/Component.hpp"


namespace siren::core
{

struct Tag final : Component
{
    std::string tag;

    explicit Tag(const std::string& tag) : tag(tag) { };
};

} // namespace siren::ecs
