#pragma once

#include "condition_variable.hpp"
#include "mutex.hpp"


namespace siren::core
{

/**
 * @class RenderThread
 * @brief Thread responsible for executing all GPU commands.
 */
class RenderThread {
public:
    using RenderTask = std::function<void()>;

    RenderThread(const RenderThread&)            = delete;
    RenderThread(RenderThread&&)                 = delete;
    RenderThread& operator=(const RenderThread&) = delete;
    RenderThread& operator=(RenderThread&&)      = delete;

    RenderThread();
    ~RenderThread();

    /// @brief Spawns a new render task.
    void spawn(RenderTask&& task);

private:
    auto run() -> void; ///< @brief Main worker loop.

    struct Inner {
        std::queue<RenderTask> tasks;
        std::thread thread;
    };

    std::atomic<bool> m_terminate;
    ConditionVariable m_condition;
    Mutex<Inner> m_inner;
};

} // namespace siren::core
