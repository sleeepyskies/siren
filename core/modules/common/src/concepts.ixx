export module siren.common:concepts;

import <concepts>;
import <string>;
import <utility>;

import :typedefs;

export namespace siren
{

/**
 * @brief Ensures a type implements a to_string() method.
 * @tparam T The type to check.
 */
template <typename T>
concept IsFormattable = requires (const T value) {
    { value.to_string() } -> std::convertible_to<std::string>;
};

template <typename T>
concept IsHashable = requires (const T value) {
    { value.hash() } -> std::convertible_to<usize>;
};

/**
 * @brief Ensures a type is an enum.
 * @tparam E The type to check.
 */
template <typename E>
concept IsEnum = std::is_enum_v<E>;

/**
 * @brief Ensures a type is an enum with a Max member for sizing.
 * @tparam E The type to check.
 * @note There is no guarantee that Max is actually the size of the enum,
 * it is just a convention.
 */
template <typename E>
concept IsSizedEnum = std::is_enum_v<E> && requires {
    { std::to_underlying(E::Max) } -> std::convertible_to<usize>;
};

/**
 * @brief Ensures the give type is callable.
 * @tparam F The type to check.
 */
template <typename F>
concept IsCallable = std::is_invocable_v<F>;

/**
 * @brief Ensures a function is a predicate.
 * @tparam F The function to check.
 */
template <typename F>
concept IsPredicate = std::is_invocable_v<F> && std::is_convertible_v<decltype(std::declval<F>()()), bool>;

/**
 * @brief Ensures that some enum has a function to stringify it.
 * @tparam T the type to check.
 */
template <IsEnum T>
concept HasErrorString = requires (const T value) {
    { to_string(value) } -> std::convertible_to<std::string>;
};

} // namespace siren
