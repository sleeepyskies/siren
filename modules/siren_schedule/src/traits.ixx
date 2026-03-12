module;

#include <tuple>

export module siren.schedule.traits;

import siren.common;

export namespace siren::schedule {

template <typename T>
class FunctionTraits { };

template <typename Ret, typename... Args>
class FunctionTraits {
    using Return                     = Ret;
    using ArgsPack                   = std::tuple<Args...>;
    static constexpr usize ArgsCount = sizeof...(Args);
};
} //  namespace siren::schedule

