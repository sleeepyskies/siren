#pragma once
#include <bits/chrono.h>


namespace siren::core
{

/**
 * @brief Tracks time from some start point. Provides some useful time related utility functions.
 */
class Timer
{
public:
    /// @brief Initializes the Timer.
    static void initialize();
    /// @brief Should be called once a frame.
    static void tick();
    /// @brief Returns the amount of time that has since this objects creation in seconds.
    static float getElapsedS();
    /// @brief Returns the amount of time that has since this objects creation in milliseconds.
    static float getElapsedMs();
    /// @brief Returns the amount of time that has passed since the previous frame in seconds.
    static float getDelta();
    /// @brief Returns the amount of time that has passed since the previous frame in milliseconds.
    static float getDeltaMS();
};

} // siren::core
