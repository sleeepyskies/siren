#pragma once

#include "condition_variable.hpp"
#include "mutex.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @class ThreadPool
 * @brief A worker pool for async task execution.
 * Manages a set of workers that handle incoming tasks.
 * Note that this should not be used for any GPU related
 * tasks, but rather CPU tasks only, @ref see GpuWorker.
 */
class ThreadPool {
public:
    /// @brief A task to be performed asynchronously.
    using Task = std::function<void()>;

    explicit ThreadPool(u32 thread_count = std::thread::hardware_concurrency() - 1); // -1 for main thread
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    /// @brief Spawns a new task to be handled asynchronously by a worker.
    void spawn(Task&& task);

private:
    void loop(); ///< @brief Main worker loop.

    /// @brief Internal data of ThreadPool.
    struct Inner {
        std::vector<std::thread> threads; ///< @brief The pool of threads.
        bool terminate = false;           ///< @brief Flag indicating pool shutdown.
        std::queue<Task> tasks;           ///< @brief All tasks waiting for a worker.
    };

    ConditionVariable m_cv; ///< @brief Used to wake up workers for a new task.
    Mutex<Inner> m_data;    ///< @brief Internal data locked behind a @ref Mutex.
};

} // namespace siren::core
