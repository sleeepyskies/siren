#pragma once

#include "spch.hpp"


namespace siren::core
{

/// @todo custom DeltaTime struct?

/**
 * @brief Tracks time from some start point. Provides some useful time related utility functions.
 */
class Time {
public:
    /// @brief Initializes the Timer.
    static auto init() -> void;
    /// @brief Should be called once a frame.
    static auto tick() -> void;
    /// @brief Returns the amount of time that has since init in seconds.
    static auto get_elapsed() -> f32;
    /// @brief Returns the amount of time that has since init in milliseconds.
    static auto get_elapsed_ms() -> f32;
    /// @brief Returns the amount of time that has passed since the previous frame in seconds.
    static auto delta() -> f32;
    /// @brief Returns the amount of time that has passed since the previous frame in milliseconds.
    static auto get_delta_ms() -> f32;
};
} // siren::core
