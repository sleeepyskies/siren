module;

#include <utility>

export module siren.common:type_traits;

import :typedefs;

export namespace siren {

template <typename T>
class FunctionTraits { };

/**
 * @brief Utility struct for determining the type of a function pointer.
 */
template <typename Ret, typename... Args>
class FunctionTraits<Ret(*)(Args...)> {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

/**
 * @brief Utility struct for determining the type of a function pointer.
 */
template <typename Class, typename Ret, typename... Args>
class FunctionTraits<Ret(Class::*)(Args...)> {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced class holding the function. */
    using ClassType = Class;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

template <typename... Args>
class TypePack { };

} // namespace siren

