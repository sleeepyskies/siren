module;

#include <string_view>
#include <libassert/assert.hpp>

export module siren.asset:asset_error;

import siren.common;

namespace siren::asset {

/** @brief Main error code used within the asset module. */
export enum class AssetErrorCode {
    BlaBla,
};

/** @brief Converts an @ref AssetErrorCode. */
export auto to_string(const AssetErrorCode e) -> std::string_view {
    switch (e) {
        case AssetErrorCode::BlaBla: return "BlaBla";
    }
    UNREACHABLE();
}

/** @brief Error type used within the asset module. */
export using AssetError = Error<AssetErrorCode>;

} // namespace siren::asset
