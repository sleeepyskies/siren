#pragma once

namespace siren::core
{
/**
 * @brief Tracks time from some start point. Provides some useful time related utility functions.
 */
class Timer
{
public:
    /// @brief Initializes the Timer.
    static void init();
    /// @brief Should be called once a frame.
    static void tick();
    /// @brief Returns the amount of time that has since this objects creation in seconds.
    static float get_elapsed();
    /// @brief Returns the amount of time that has since this objects creation in milliseconds.
    static float get_elapsed_ms();
    /// @brief Returns the amount of time that has passed since the previous frame in seconds.
    static float get_delta();
    /// @brief Returns the amount of time that has passed since the previous frame in milliseconds.
    static float get_delta_ms();
};
} // siren::core
