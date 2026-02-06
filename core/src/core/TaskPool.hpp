#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @class TaskPool
 * @brief A worker pool for async task execution.
 * Manages a set of workers that handle incoming tasks.
 */
class TaskPool {
public:
    /// @brief A task to be performed asynchronously.
    using Task = std::function<void()>;

    explicit TaskPool(u32 thread_count = std::thread::hardware_concurrency() - 1); // -1 for main thread
    ~TaskPool();

    TaskPool(const TaskPool&)            = delete;
    TaskPool(TaskPool&&)                 = delete;
    TaskPool& operator=(const TaskPool&) = delete;
    TaskPool& operator=(TaskPool&&)      = delete;

    /// @brief Spawns a new task to be handled asynchronously by a worker.
    template <typename Fn>
        requires(std::is_invocable_v<Fn>)
    void spawn(Fn&& task) {
        #ifdef SIREN_SINGLE_THREADED
        task();
        #else
        std::unique_lock lock(m_mutex);
        m_tasks.push(std::move(task));
        lock.unlock(); // unlock before notifying so the thread doesn't have to wait
        m_cv.notify_one();
        #endif
    }

private:
    void loop(); ///< @brief Main worker loop.

    std::vector<std::thread> m_threads; ///< @brief The pool of threads.
    std::mutex m_mutex;                 ///< @brief A mutex for guarding access to the task queue.
    std::queue<Task> m_tasks;           ///< @brief All tasks waiting for a worker.
    std::condition_variable m_cv;       ///< @brief Used to wake up workers for a new task.
    bool m_terminate = false;           ///< @brief Flag indicating pool shutdown.
};
}
