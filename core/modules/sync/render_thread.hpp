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
    using RenderTask = std::move_only_function<void()>;

    RenderThread(const RenderThread&)            = delete;
    RenderThread(RenderThread&&)                 = delete;
    RenderThread& operator=(const RenderThread&) = delete;
    RenderThread& operator=(RenderThread&&)      = delete;

    RenderThread();
    ~RenderThread();

    /**
     * @brief Spawns a new task to be computed on the RenderThread.
     * @param task The task to perform.
     */
    auto spawn(RenderTask&& task) -> void;

    /**
     * @brief Blocks the calling thread until the RenderThread has no tasks left.
     */
    auto wait_until_idle() const noexcept -> void;

private:
    /// @brief Main worker loop.
    auto run() -> void;

    /// @brief Holds inner data.
    struct Inner {
        /// @brief The queue of @ref RenderTask's to perform.
        std::queue<RenderTask> tasks;
        /// @brief The actual worker thread.
        std::thread thread;
    };

    /// @brief Flag indicating if the thread should terminate.
    std::atomic_bool m_terminate;
    /// @brief Number of tasks left to process. A task being process still counts towards this value.
    std::atomic<usize> m_task_count;
    /// @brief Used to sleep and wakeup the thread.
    ConditionVariable m_condition;
    /// @brief Locked inner data.
    Mutex<Inner> m_inner;
};

} // namespace siren::core
