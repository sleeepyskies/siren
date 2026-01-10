// ReSharper disable CppNonExplicitConvertingConstructor
// ReSharper disable CppPossiblyUninitializedMember
#pragma once

#include "utilities/SirenAssert.hpp"


namespace siren::core
{
// @formatter:off
namespace internal { struct NoneT { }; };
inline constexpr internal::NoneT None;
// @formatter:on

template <typename T>
class Option;

template <typename>
inline constexpr bool is_option_v = false;
template <typename T>
inline constexpr bool is_option_v<Option<T>> = true;

template <typename T>
struct Some
{
    Some(T&& val) : val(std::move(val)) { }

    Some(const T& val) : val(val) { }

    T val;
};

template <typename T>
class Option
{
public:
    ~Option() { erase(); }

    Option(internal::NoneT) : m_has_value(false) { }

    Option(Some<T>&& s) : m_has_value(true) { new(m_data) T(std::move(s.val)); }

    Option(const Some<T>& s) : m_has_value(true) { new(m_data) T(s.val); }

    [[nodiscard]]
    bool is_some() const { return m_has_value; }

    [[nodiscard]]
    bool is_none() const { return !m_has_value; }

    [[nodiscard]]
    explicit operator bool() const { return is_some(); }

    [[nodiscard]]
    T& unwrap()
    {
        SirenAssert(m_has_value, "Cannot unwrap None.");
        return *reinterpret_cast<T*>(m_data);
    }
    [[nodiscard]]
    const T& unwrap() const
    {
        SirenAssert(m_has_value, "Cannot unwrap None.");
        return *reinterpret_cast<const T*>(m_data);
    }

    void erase()
    {
        if (!m_has_value) { return; }
        m_has_value = false;
        reinterpret_cast<T*>(m_data)->~T();
    }

    [[nodiscard]]
    T* operator->() { return &unwrap(); }
    [[nodiscard]]
    T& operator*() { return unwrap(); }
    [[nodiscard]]
    const T* operator->() const { return &unwrap(); }
    [[nodiscard]]
    const T& operator*() const { return unwrap(); }

    /// @brief Takes a function that takes the inner type of this option, and returns
    /// a new type. The type is wrapped in a new Option<U>
    template <typename MapFn>
        requires(std::is_invocable_v<MapFn, T>)
    auto map(MapFn&& fn) -> Option<std::invoke_result_t<MapFn, T>>
    {
        if (is_none()) { return None; }
        return Some(fn(unwrap()));
    }

    /// @brief Takes a function that takes the inner type of this option, and returns
    /// a new type wrapped in an Option.
    template <typename FlatMapFn>
        requires(std::is_invocable_v<FlatMapFn, T> && is_option_v<std::invoke_result_t<FlatMapFn, T>>)
    auto flat_map(FlatMapFn&& fn) -> std::invoke_result_t<FlatMapFn, T> { return is_none() ? None : fn(unwrap()); }

    /// @brief Returns either the contained value if present, or the provided fallback value.
    T or_else(const T& fallback) { return is_some() ? unwrap() : fallback; }

private:
    bool m_has_value;                  ///< Flag indicating if this Option<T> contains a value
    alignas(T) char m_data[sizeof(T)]; ///< The contained value
};
}
