#pragma once

#include "mutex.hpp"


namespace siren::core
{

/**
 * @class RenderThread
 * @brief Thread responsible for executing all GPU commands.
 */
class RenderThread {
public:
    using RenderTask = int;

private:
    Mutex<std::queue<RenderTask>> m_tasks;
};

} // namespace siren::core
