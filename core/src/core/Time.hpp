#pragma once

namespace siren::core
{

class Time
{
public:
    float delta();
    float elapsed() const;

private:
    float m_previousFrame = 0;
    float m_elapsed       = 0;
};

} // namespace siren::core
