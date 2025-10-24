/**
 * @file TimeModule.hpp
 */
#pragma once

#include "Timer.hpp"
#include "core/Module.hpp"

namespace siren::core
{

/**
 * @brief TimeModule provides functionality for retrieving time values.
 */
class TimeModule final : public Module
{
public:
    bool initialize() override;
    void shutdown() override {}

    float delta();
    float elapsedS() const;
    float elapsedMs() const;

    const char* getName() override;

private:
    float m_previousFrame = 0;
    float m_elapsed       = 0;

    Own<Timer> m_timer = nullptr;
};

} // namespace siren::core
