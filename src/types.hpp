#pragma once
#include <memory>

template <typename T> using ref = std::shared_ptr<T>;
template <typename T, typename... Args> ref<T> makeRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T> using uref = std::unique_ptr<T>;
template <typename T, typename... Args> uref<T> makeUref(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}
