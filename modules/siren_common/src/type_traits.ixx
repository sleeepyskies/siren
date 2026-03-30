module;

#include <utility>

export module siren.common:type_traits;

import :typedefs;

export namespace siren {

template <typename T>
class FunctionTraits { };

/**
 * @brief Utility struct for determining the type of a function.
 */
template <typename Ret, typename... Args>
class FunctionTraits {
    /** @brief The deduced return type of the function. */
    using Return = Ret;
    /** @brief The deduced function argument types of the function. */
    using ArgsPack = std::tuple<Args...>;
    /** @brief The deduced argument count of the function. */
    static constexpr usize ArgsCount = sizeof...(Args);
};

template <typename... Args>
class TypePack { };

} // namespace siren

