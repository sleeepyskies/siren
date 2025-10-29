#pragma once

namespace siren::utilities
{

/**
 * @brief A randomly assigned 64-bit integer ID.
 */
class UUID
{
public:
    /// @brief Creates an invalid UUID. This should ideally not be used, instead use UUID::invalid and UUID::create.
    /// This is only made public to allow default constructing.
    UUID() { }

    UUID(const UUID&)            = default;
    UUID& operator=(const UUID&) = default;

    /// @brief Sets the seed
    static void setSeed(u64 seed);
    /// @brief Constructs and returns a valid AssetHandle.
    static UUID create();
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
    u64 m_uuid = 0;

    /// @brief Used only internally to construct an invalid AssetHandle. Is useful for default
    /// member variables that are overridden by constructor parameters to indicate a failure.
    explicit UUID(u64 uuid);

    friend struct std::hash<UUID>;      // allow access for hashing
    friend struct std::formatter<UUID>; // allow access for formatting
};

} // namespace siren::utilities

// make UUID hashable and usable as a key in hash maps
template <>
struct std::hash<siren::utilities::UUID>
{
    size_t operator()(const siren::utilities::UUID& handle) const noexcept
    {
        return ::std::hash<uint64_t>{ }(handle.m_uuid);
    }
};

template <>
struct std::formatter<siren::utilities::UUID> : std::formatter<uint64_t>
{
    auto format(const siren::utilities::UUID& handle, std::format_context& ctx) const
    {
        return std::formatter<uint64_t>::format(handle.m_uuid, ctx);
    }
};
