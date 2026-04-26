module;

#include <utility>
#include <tuple>

export module siren.common:type_traits;

import :typedefs;

export namespace siren {

template <typename... Args>
struct TypePack { };

template <typename T>
struct ToTypePack;

template <typename... Args>
struct ToTypePack<std::tuple<Args...>> {
    using Type = TypePack<Args...>;
};

/**
 * @brief Primary template for FunctionTraits.
 * Redirects to operator() for functors/lambdas.
 */
template <typename T>
struct FunctionTraits : FunctionTraits<decltype(&T::operator())> { };

/**
 * @brief Utility struct for determining the type of a function pointer.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(*)(Args...)> {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

/**
 * @brief Utility struct for determining the type of a member function pointer (const).
 * This is the specialization typically used by lambdas.
 */
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...) const> {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced class holding the function. */
    using ClassType = Class;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

/**
 * @brief Utility struct for determining the type of a member function pointer (non-const).
 */
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...)> {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced class holding the function. */
    using ClassType = Class;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

} // namespace siren
