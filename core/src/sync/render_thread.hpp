#pragma once

#include "condition_variable.hpp"
#include "mutex.hpp"


namespace siren::core
{

/**
 * @class RenderThread
 * @brief Manages the execution of all GPU operations and graphics API calls.
 */
class RenderThread {
public:
    /// @brief A command to be computed on the RenderThread.
    /// @todo Does using std::function incur too much overhead?
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

    /// @brief Holds inner data.
    struct Inner {
        std::queue<RenderTask> tasks; ///< @brief The queue of @ref RenderTask's to perform.
        std::thread thread;           ///< @brief The actual worker thread.
    };

    std::atomic<bool> m_terminate; ///< @brief Flag indicating if the thread should terminate.
    ConditionVariable m_condition; ///< @brief Used to sleep and wakeup the thread.
    Mutex<Inner> m_inner;          ///< @brief Locked inner data.
};

} // namespace siren::core
