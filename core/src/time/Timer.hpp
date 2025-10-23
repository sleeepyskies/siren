#pragma once

#include "utilities/spch.hpp"

namespace siren::core
{

class Timer
{
public:
    virtual ~Timer()                          = default;
    virtual float getTimeSeconds() const      = 0;
    virtual float getTimeMilliseconds() const = 0;

    static Own<Timer> create();
};

} // namespace siren::core
