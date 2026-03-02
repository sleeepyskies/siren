export module siren.common:error;

import <optional>;
import <format>;
import <string>;
import :concepts;

export namespace siren
{

/**
 * @class Error
 * @brief An error container for the siren engine.
 * Allows for each system to define its own error types.
 *
 * En example of creating a new Error type is:
 * @code
 * import siren.common;
 *
 * enum class MyErrCode {
 *      Something,
 *      SomethingElse,
 * };
 *
 * auto to_string(const MyErrCode code) const -> std::string_view {
 *      switch (code) {
 *          case MyErrCode::Something: return "Something";
 *          case MyErrCode::SomethingElse : return "SomethingElse";
 *          UNREACHABLE;
 *      };
 * }
 *
 * using MyError = Error<MyErrCode>;
 * @endcode
 *
 * @tparam ErrCode An enum type that has a function to stringify it, and that
 * lists all errors for this specialization.
 */
template <HasErrorString ErrCode>
class Error {
public:
    explicit Error(
        const ErrCode code,
        const std::optional<std::string>& msg = std::nullopt
    ) : code(code), msg(msg) { }

    auto to_string() const noexcept -> std::string {
        return std::format("[Error::{}]", to_string(code), msg.value_or(""));
    }

private:
    ErrCode code;
    std::optional<std::string> msg;
};

class AnyError {
public:
    template <typename T>
        requires(std::derived_from<T, Error>)
    AnyError();

private:
};
} // namespace siren
