#pragma once

#include <string_view>
#include "hashed_string.hpp"

#if defined __clang__ || defined __GNUC__
#    define SIREN_PRETTY_FUNCTION __PRETTY_FUNCTION__
#    define SIREN_PRETTY_FUNCTION_PREFIX '='
#    define SIREN_PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
#    define SIREN_PRETTY_FUNCTION __FUNCSIG__
#    define SIREN_PRETTY_FUNCTION_PREFIX '<'
#    define SIREN_PRETTY_FUNCTION_SUFFIX '>'
#endif

namespace siren
{
namespace detail
{

template <typename Type>
[[nodiscard]] constexpr const char* pretty_function() noexcept {
#if defined SIREN_PRETTY_FUNCTION
    return SIREN_PRETTY_FUNCTION;
#else
    return "";
#endif
}

template <typename Type>
[[nodiscard]] constexpr std::string_view stripped_type_name() noexcept {
#if defined SIREN_PRETTY_FUNCTION
    const std::string_view full_name{ pretty_function<Type>() };
    const auto first = full_name.find_first_not_of(' ', full_name.find_first_of(SIREN_PRETTY_FUNCTION_PREFIX) + 1);
    return full_name.substr(first, full_name.find_last_of(SIREN_PRETTY_FUNCTION_SUFFIX) - first);
#else
    return std::string_view{ };
#endif
}

// Renamed to 'type_name_helper' to prevent symbol collision in Clangd's indexer
template <typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] constexpr std::string_view type_name_helper(int) noexcept {
    return stripped_type_name<Type>();
}

template <typename Type>
[[nodiscard]] std::string_view type_name_helper(char) noexcept {
    static const std::string_view value = stripped_type_name<Type>();
    return value;
}
} // namespace detail

template <typename Type>
struct TypeName final {
    [[nodiscard]] static constexpr std::string_view value() noexcept {
        return detail::type_name_helper<Type>(0);
    }

    [[nodiscard]] constexpr operator std::string_view() const noexcept {
        return detail::type_name_helper<Type>(0);
    }
};

template <typename Type>
struct TypeHash final {
    [[nodiscard]] static constexpr core::HashedString value() noexcept {
        // Explicitly use .data() on the forced string_view type
        return core::HashedString{ detail::type_name_helper<Type>(0).data() };
    }
};

} // namespace siren
