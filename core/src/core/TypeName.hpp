/**
 * @file TypeName.hpp
 * @brief Contains meta helper functions for getting the name
 * of a type, as well as hashing as type.
 *
 * Inspired by entt.
 */
#pragma once

#include <string_view>

#include "HashedString.hpp"

#if defined __clang__ || defined __GNUC__
#    define SirenPrettyFunction __PRETTY_FUNCTION__
#    define SirenPrettyFunctionPrefix '='
#    define SirenPrettyFunctionSuffix ']'
#elif defined _MSC_VER
#    define SirenPrettyFunction __FUNCSIG__
#    define SirenPrettyFunctionPrefix '<'
#    define SirenPrettyFunctionSuffix '>'
#endif

namespace siren::core
{
namespace internal
{
template <typename Type>
[[nodiscard]]
constexpr const char* pretty_function() noexcept
{
    #if defined SirenPrettyFunction
    return SirenPrettyFunction;
    #else
    return "";
    #endif
}

template <typename Type>
[[nodiscard]]
constexpr auto stripped_type_name() noexcept
{
    #if defined SirenPrettyFunction
    const std::string_view full_name{ pretty_function<Type>() };
    const auto first = full_name.find_first_not_of(' ', full_name.find_first_of(SirenPrettyFunctionPrefix) + 1);
    auto value       = full_name.substr(first, full_name.find_last_of(SirenPrettyFunctionSuffix) - first);
    return value;
    #else
    return std::string_view{ };
    #endif
}

template <typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]]
constexpr std::string_view type_name(int) noexcept
{
    constexpr auto value = stripped_type_name<Type>();
    return value;
}

template <typename Type>
[[nodiscard]]
std::string_view type_name(char) noexcept
{
    static const auto value = stripped_type_name<Type>();
    return value;
}
} // namespace internal

template <typename Type>
struct TypeName final
{
    [[nodiscard]]
    static constexpr std::string_view value() noexcept
    {
        return internal::type_name<Type>(0);
    }

    [[nodiscard]]
    explicit constexpr operator std::string_view() const noexcept
    {
        return value();
    }
};

template <typename Type>
struct TypeHash final
{
    [[nodiscard]]
    static constexpr HashedString value() noexcept
    {
        return HashedString{ internal::type_name<Type>(0) };
    }
};
}
