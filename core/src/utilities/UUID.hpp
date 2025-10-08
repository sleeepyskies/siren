#pragma once

namespace siren::utilities
{

/**
 * @brief A randomly assigned 64-bit integer ID.
 */
class UUID
{
public:
    /// @brief Assigns a random uuid on construction
    UUID();
    UUID(const UUID&)            = default;
    UUID& operator=(const UUID&) = default;

    /// @brief Constructs and returns an invalid AssetHandle that does not reference an asset
    static UUID invalid();

    /// @brief Invalidates this UUID. Cannot be undone, so use carefully!
    void invalidate();

    bool operator==(const UUID&) const;
    bool operator<(const UUID&) const; // usable in ordered data structures
    explicit operator bool() const;

private:
    /// @brief We use just a 64-bit handle as this is far more than enough for our use case.
    /// Around 607 million assets would need to be imported for a 0.1% chance of a duplicate uuid
    uint64_t m_uuid = 0;

    /// @brief Used only internally to construct an invalid AssetHandle. Is useful for default
    /// member variables that are overridden by constructor parameters to indicate a failure.
    explicit UUID(const uint64_t uuid);

    friend struct std::hash<UUID>; // allow access for hashing
};

} // namespace siren::utilities

// make uuid hashable and usable as a key in hash maps
template <>
struct std::hash<siren::utilities::UUID> {
    size_t operator()(const siren::utilities::UUID& handle) const noexcept
    {
        return ::std::hash<uint64_t>{}(handle.m_uuid);
    }
};
