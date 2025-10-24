#pragma once

#include "time/Timer.hpp"

namespace siren::platform
{

class WindowsTimer final : public core::Timer
{
public:
    float getTimeSeconds() const override;
    float getTimeMilliseconds() const override;
};

} // namespace siren::platform
