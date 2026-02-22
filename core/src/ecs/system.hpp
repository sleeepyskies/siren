#pragma once

#include "core/spch.hpp"


namespace siren::core
{

/**
 * @brief A System is just some functor that takes
 * any number of arguments, and returns void.
 */
template <typename T, typename... Args>
concept System = requires (T v, Args... args)
{
    { v(args...) } -> std::same_as<void>;
};

} // namespace siren::core
