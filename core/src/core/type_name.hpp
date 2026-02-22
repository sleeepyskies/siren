/**
 * @file type_name.hpp
 * @brief Contains meta helper functions for getting the name
 * of a type, as well as hashing as type.
 *
 * Inspired by entt.
 */
#pragma once

#include <string_view>
#include "hashed_string.hpp"

#if defined __clang__ || defined __GNUC__
#    define SirenPrettyFunction __PRETTY_FUNCTION__
#    define SirenPrettyFunctionPrefix '='
#    define SirenPrettyFunctionSuffix ']'
#elif defined _MSC_VER
#    define SirenPrettyFunction __FUNCSIG__
#    define SirenPrettyFunctionPrefix '<'
#    define SirenPrettyFunctionSuffix '>'
#endif

namespace siren
{
namespace detail
{
template <typename Type>
[[nodiscard]]
constexpr const char* pretty_function() noexcept {
    #if defined SirenPrettyFunction
    return SirenPrettyFunction;
    #else
    return "";
    #endif
}

template <typename Type>
[[nodiscard]]
constexpr auto stripped_type_name() noexcept {
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
constexpr auto type_name(int) noexcept -> std::string_view {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}

template <typename Type>
[[nodiscard]]
auto type_name(char) noexcept -> std::string_view {
    static const auto value = stripped_type_name<Type>();
    return value;
}
} // namespace detail

/**
 * @brief Takes some type and returns its name as a string.
 * @tparam Type The type to retrieve the name for.
 */
template <typename Type>
struct TypeName final {
    [[nodiscard]]
    static constexpr auto value() noexcept -> std::string_view {
        return detail::type_name<Type>(0);
    }
};

/**
 * @brief Takes a type, and returns a unique hash value for this type.
 * @tparam Type The type to hash as a string.
 */
template <typename Type>
struct TypeHash final {
    [[nodiscard]]
    static constexpr auto value() noexcept -> core::HashedString {
        return core::HashedString{ detail::type_name<Type>(0).data() };
    }
};

constexpr auto type_name(const auto& instance) -> std::string_view {
    return TypeName<decltype(instance)>::value();
}

template <typename Type>
constexpr auto type_name(const auto& instance) -> std::string_view {
    return TypeName<Type>::value();
}

constexpr auto type_hash(const auto& instance) -> core::HashedString {
    return TypeHash<decltype(instance)>::value();
}

template <typename Type>
constexpr auto type_name(const auto& instance) -> core::HashedString {
    return TypeHash<Type>::value();
}

} // namespace siren
