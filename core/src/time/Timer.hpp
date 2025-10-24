/**
 * @file Timer.hpp
 */
#pragma once

namespace siren::core
{

/**
 * @brief The Timer provides functions for retrieving the elapsed time since Siren start.
 */
class Timer
{
public:
    virtual ~Timer()                          = default;
    /// @brief Returns the elapsed time in seconds.
    virtual float getTimeSeconds() const      = 0;
    /// @brief Returns the elapsed time in milliseconds.
    virtual float getTimeMilliseconds() const = 0;
};

} // namespace siren::core
