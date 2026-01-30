// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once
#include "Error.hpp"
#include "SirenAssert.hpp"

// todo: T& as well

namespace siren::core
{
struct Unit { };

inline constexpr Unit unit;

template <typename E>
struct Err
{
    Err(E&& e) : e(std::move(e)) { }
    Err(const E& e) : e(e) { }
    E e;
};

template <typename T>
struct Ok
{
    Ok(T&& t) : t(std::move(t)) { }
    Ok(const T& t) : t(t) { }
    T t;
};

template <typename T, typename E>
using ResultData = std::variant<Ok<T>, Err<E>>;

template <typename T = Unit, typename E = Error>
class Result
{

public:
    Result() = delete;

    Result(const Err<E>& e) : m_data(e) { }
    Result(Err<E>&& e) : m_data(std::move(e)) { }
    Result(const Ok<T>& t) : m_data(t) { }
    Result(Ok<T>&& t) : m_data(std::move(t)) { }

    Result(const Result& other) : m_data(other.m_data) { }
    auto operator=(const Result& other) -> Result& {
        if (this != &other) { m_data = other.m_data; }
        return *this;
    }
    Result(Result&& other) noexcept : m_data(std::move(other.m_data)) { };
    auto operator=(Result&& other) noexcept -> Result& {
        if (this != &other) { m_data = std::move(other.m_data); }
        return *this;
    }

    [[nodiscard]]
    constexpr auto is_ok() const noexcept -> bool { return std::holds_alternative<Ok<T>>(m_data); }
    [[nodiscard]]
    constexpr auto is_err() const noexcept -> bool { return !is_ok(); }
    [[nodiscard]]
    constexpr auto unwrap() noexcept -> T& {
        SIREN_ASSERT(is_ok(), "Cannot unwrap Err.");
        return std::get<Ok<T>>(m_data).t;
    }
    [[nodiscard]]
    constexpr auto unwrap() const noexcept -> const T& {
        SIREN_ASSERT(is_err(), "Cannot unwrap Ok.");
        return std::get<Ok<T>>(m_data).t;
    }
    [[nodiscard]]
    constexpr auto unwrap_err() noexcept -> T& {
        SIREN_ASSERT(is_err(), "Cannot unwrap_err Ok.");
        return std::get<Err<E>>(m_data).e;
    }
    [[nodiscard]]
    constexpr auto unwrap_err() const noexcept -> const E& {
        SIREN_ASSERT(is_err(), "Cannot unwrap_err Ok.");
        return std::get<Err<E>>(m_data).e;
    }

    /// @brief Calls the provided function on the inner ok value if it exists.
    template <typename InspectFn>
        requires(std::is_invocable_v<InspectFn, T>)
    [[nodiscard]]
    constexpr auto inspect(InspectFn&& fn) const noexcept -> Result {
        if (is_ok()) { fn(unwrap()); }
        return *this;
    }
    /// @brief Calls the provided function on the inner error value if it exists.
    template <typename InspectFn>
        requires(std::is_invocable_v<InspectFn, T>)
    [[nodiscard]]
    constexpr auto inspect_err(InspectFn&& fn) const noexcept -> Result {
        if (is_err()) { fn(unwrap_err()); }
        return *this;
    }

private:
    ResultData<T, E> m_data;
};

template <typename E>
Result(Err<E>) -> Result<Unit, E>;

} // namespace siren::Core

template <typename T, typename E>
struct std::formatter<siren::core::Result<T, E>> : std::formatter<std::string_view>
{
    auto format(const siren::core::Result<T, E>& res, std::format_context& ctx) const {
        if (res.is_ok()) { return std::format_to(ctx.out(), "Ok({})", res.unwrap()); }
        return std::format_to(ctx.out(), "Err({})", res.unwrap_err());
    }
};
