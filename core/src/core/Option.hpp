// ReSharper disable CppNonExplicitConvertingConstructor
// ReSharper disable CppPossiblyUninitializedMember
#pragma once

#include "Core.hpp"
#include "SirenAssert.hpp"
#include "TypeName.hpp"


namespace siren::core
{
// @formatter:off
template <typename T>
class Option;

namespace internal { struct NoneT { }; }
inline constexpr internal::NoneT None;

template <typename T>
struct Some
{
    constexpr Some(T&& val) noexcept : val(std::move(val)) { }
    constexpr Some(const T& val) noexcept : val(val) { }
    T val;
};

template <typename T>
struct Some<T&>
{
    constexpr Some(const T& ref) noexcept : ref(ref) { }
    T& ref;
};

template <typename>
inline constexpr bool is_option_v = false;
template <typename T>
inline constexpr bool is_option_v<Option<T>> = true;

// @formatter:on

namespace internal
{
/// @brief Base mixin class to avoid code duplication in Option<T> and Option<T&>.
// god wtf is this templating im dying
template <
    template <typename> typename Container, ///< @brief The Option container without the T
    typename T,                             ///< @brief The wrapped type.
    typename Derived                        ///< @brief The Option<T> type.
>
struct OptionBase {
private:
    constexpr auto derived() noexcept -> Derived& { return *static_cast<Derived*>(this); }
    constexpr auto derived() const noexcept -> const Derived& { return *static_cast<const Derived*>(this); }

public:
    [[nodiscard]]
    constexpr auto is_none() const noexcept -> bool { return !derived().is_some(); }
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept { return derived().is_some(); }
    [[nodiscard]]
    auto operator->() -> T* { return &derived().unwrap(); }
    [[nodiscard]]
    auto operator*() -> T& { return derived().unwrap(); }
    [[nodiscard]]
    auto operator->() const -> const T* { return &derived().unwrap(); }
    [[nodiscard]]
    auto operator*() const -> const T& { return derived().unwrap(); }

    /// @brief Takes a function that takes the inner type of this option, and returns
    /// a new type. The type is wrapped in a new Option<U>
    /// @todo Support r-value arguments? like obj.map([](auto&& value){}); ?
    template <typename MapFn>
        requires(std::is_invocable_v<MapFn, T>)
    [[nodiscard]]
    constexpr auto map(MapFn&& fn) const -> Container<std::invoke_result_t<MapFn, T>> {
        if (is_none()) { return None; }
        return Some(fn(derived().unwrap()));
    }

    /// @brief Takes a function that takes the inner type of this option, and returns
    /// a new type wrapped in an Option.
    template <typename AndThenFn>
        requires(std::is_invocable_v<AndThenFn, T> && is_option_v<std::invoke_result_t<AndThenFn, T>>)
    [[nodiscard]]
    constexpr auto and_then(AndThenFn&& fn) const -> std::invoke_result_t<AndThenFn, T> {
        return is_none() ? None : fn(derived().unwrap());
    }

    /// @brief Returns either the contained value if present, or the provided fallback value.
    [[nodiscard]]
    constexpr auto value_or(const T& fallback) const noexcept -> T {
        return derived().is_some() ? derived().unwrap() : fallback;
    }

    /// @brief Returns either the contained value if present, or the value returned by the provided functor.
    template <typename OrElseFn>
        requires(std::is_invocable_v<OrElseFn>)
    [[nodiscard]]
    constexpr auto or_else(OrElseFn&& fn) const noexcept -> std::invoke_result_t<OrElseFn> {
        return derived().is_some() ? derived().unwrap() : fn();
    }

    /// @brief Calls the provided function if the inner value exists.
    template <typename InspectFn>
        requires(std::is_invocable_v<InspectFn, T>)
    constexpr auto inspect(InspectFn&& fn) -> Derived& {
        if (!is_none()) { fn(derived().unwrap()); }
        return derived();
    }
};
};


/**
 * @brief Wraps some value, providing optionality. The value either exists, or does not.
 * Option<T> is owning and stack allocated.
 * @tparam T The underlying wrapped type.
 */
template <typename T>
class Option final : public internal::OptionBase<Option, T, Option<T>> {
public:
    Option() : m_has_value(false) { }
    Option(internal::NoneT) : m_has_value(false) { }
    Option(Some<T>&& s) : m_has_value(true) { new(m_data) T(std::move(s.val)); }
    Option(const Some<T>& s) : m_has_value(true) { new(m_data) T(s.val); }
    ~Option() noexcept { erase(); }
    Option(const Option& other) : m_has_value(other.m_has_value) { if (other) { new(m_data) T(other.unwrap()); } }
    Option& operator=(const Option& other) {
        if (this == &other) { return *this; }
        if (other) {
            T tmp(other.unwrap());
            erase();
            new(m_data) T(std::move(tmp));
        } else {
            erase();
        }
        m_has_value = other.m_has_value;
        return *this;
    }

    Option(Option&& other) noexcept : m_has_value(other.m_has_value) {
        if (other.m_has_value) {
            new(m_data) T(std::move(other.unwrap()));
            other.erase();
        }
    }

    Option& operator=(Option&& other) noexcept {
        if (this != &other) {
            erase();
            m_has_value = other.m_has_value;
            new(m_data) T(std::move(other.unwrap()));
            other.erase();
        }
        return *this;
    }

    [[nodiscard]]
    auto is_some() const noexcept -> bool { return m_has_value; }

    [[nodiscard]]
    auto unwrap() -> T& {
        SIREN_ASSERT(m_has_value, "Cannot unwrap None.");
        return *reinterpret_cast<T*>(m_data);
    }
    [[nodiscard]]
    auto unwrap() const -> const T& {
        SIREN_ASSERT(m_has_value, "Cannot unwrap None.");
        return *reinterpret_cast<const T*>(m_data);
    }

    auto erase() noexcept -> void {
        if (!m_has_value) { return; }
        m_has_value = false;
        reinterpret_cast<T*>(m_data)->~T();
    }

private:
    bool m_has_value; ///< @brief Flag indicating if this Option<T> contains a value
    union {
        alignas(T) char m_data[sizeof(T)]; ///< @brief The contained value
        T t;                               ///< @brief Helps debugger.
    };
};

/**
 * @brief Wraps some value, providing optionality. The value either exists, or does not.
 * Wraps a T&, and is as such not owning.
 * @tparam T The underlying wrapped type.
 */
template <typename T>
class Option<T&> final : public internal::OptionBase<Option, T&, Option<T&>> {
public:
    ~Option() noexcept { erase(); }

    Option(const Option&)             = default;
    Option& operator=(const Option&)  = default;
    Option(Option&& other) noexcept   = default;
    Option& operator=(Option&& other) = default;

    Option(internal::NoneT) : m_ptr(nullptr) { }

    Option(Some<T&>&& s) = delete; /// disallow taking address of a temporary value, bc lol.

    Option(const Some<T&>& s) : m_ptr(&s.ref) { }

    [[nodiscard]]
    auto is_some() const noexcept -> bool { return m_ptr; }

    [[nodiscard]]
    auto unwrap() -> T& {
        SIREN_ASSERT(m_ptr, "Cannot unwrap None.");
        return *m_ptr;
    }
    [[nodiscard]]
    auto unwrap() const -> const T& {
        SIREN_ASSERT(m_ptr, "Cannot unwrap None.");
        return *m_ptr;
    }

    auto erase() noexcept -> void { m_ptr = nullptr; }

private:
    T* m_ptr; ///< @brief Pointer to the stored value. nullptr indicates an empty Option.
};
}


template <typename T>
struct std::formatter<siren::core::Option<T>> : std::formatter<std::string_view> {
    auto format(const siren::core::Option<T>& opt, std::format_context& ctx) const {
        if (opt) { return std::format_to(ctx.out(), "Some({})", opt.unwrap()); }
        return std::format_to(ctx.out(), "None<{}>", siren::core::TypeName<T>::value());
    }
};

template <typename T>
struct std::formatter<siren::core::Option<T&>> : std::formatter<std::string_view> {
    auto format(const siren::core::Option<T&>& opt, std::format_context& ctx) const {
        if (opt) { return std::format_to(ctx.out(), "Some({})", opt.unwrap()); }
        return std::format_to(ctx.out(), "None<{}>", siren::core::TypeName<T>::value());
    }
};
