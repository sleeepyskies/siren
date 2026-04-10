module;

#include <concepts>

export module siren.asset:asset;

namespace siren::asset {

/**
 * @brief The base asset class.
 *
 * Provides no functionality, other than being a label to identify if an
 * object is an asset or not. This is used by the AssetServer and AssetPools
 * to ensure type safety and prevent loading of non-asset types.
 */
export struct Asset { };

/** @brief Ensures that A is derived from Asset. */
export template <typename A>
concept IsAsset = std::derived_from<A, Asset>;

} // namespace siren::asset
