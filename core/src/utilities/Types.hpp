#pragma once

#include <memory>
#include <optional>

namespace siren
{

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> makeRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using Uref = std::unique_ptr<T>;

template <typename T, typename... Args>
Uref<T> makeUref(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

using Byte = uint8_t;

template <typename T>
using Maybe = std::optional<T>;

constexpr std::nullopt_t Nothing = std::nullopt;

} // namespace siren
