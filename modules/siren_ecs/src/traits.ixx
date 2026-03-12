module;

#include <type_traits>
#include <tuple>

export module siren.ecs.traits;

import siren.ecs.system_params;
import siren.common;

namespace siren::ecs {

export {
    template <typename T>
    struct IsQuery : std::false_type { };

    template <typename... Args>
    struct IsQuery<Query<Args...>> : std::true_type { };

    template <typename... Args>
    inline constexpr bool IsQuery_v = IsQuery<Args...>::value;

    template <typename T>
    struct QueryTraits { };

    template <typename... Args>
    struct QueryTraits<Query<Args...>> {
        using ArgsTuple                  = std::tuple<Args...>;
        static constexpr usize ArgsCount = sizeof...(Args);
    };
}


export {
    template <typename T>
    struct IsResource : std::false_type { };

    template <typename T>
    struct IsResource<Resource<T>> : std::true_type { };

    template <typename T>
    inline constexpr bool IsResource_v = IsResource<T>::value;

    template <typename T>
    struct ResourceTraits { };

    template <typename T>
    struct ResourceTraits<Resource<T>> {
        using Inner = T;
    };
}

} //namespace siren::ecs
