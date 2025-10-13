#pragma once

#include "utilities/spch.hpp"

namespace siren::core
{

class PathRegistry
{
public:
    /// @brief Initializes the PathRegistry and finds the project and engine root
    PathRegistry();
    ~PathRegistry() = default;

private:
    Path m_engineRoot{};
    Path m_projectRoot{};
};

} // namespace siren::core
