#pragma once

#include "condition_variable.hpp"
#include "mutex.hpp"
#include "core/spch.hpp"
#include "core/logger.hpp"


namespace siren::core
{

/**
 * @class ThreadPool
 * @brief A worker pool for async task execution.
 * Manages a set of workers that handle incoming tasks.
 * Note that this should not be used for any GPU related
 * tasks, but rather CPU tasks only, @ref see GpuWorker.
 */
class ThreadPool : WithLogger<SystemLogger::Core> {
public:
    /// @brief A task to be performed asynchronously.
    using Task = std::function<void()>;

    /**
     * @brief Creates a new threadpool.
     * @param thread_count The number of threads to create.
     * Takes the number of available hardware threads by
     * default. A negative number creates hardware_threads - x
     * threads, with a minimum of 1.
     * @note If SIREN_SINGLE_THREADED is defined, no threads
     * will be created and all spawned tasks will be executed
     * on the main thread.
     */
    explicit ThreadPool(i32 thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    /// @brief Spawns a new task to be handled asynchronously by a worker.
    void spawn(Task&& task);

private:
    void run(); ///< @brief Main worker loop.

    /// @brief Internal data of ThreadPool.
    struct Inner {
        std::vector<std::thread> threads; ///< @brief The pool of threads.
        std::queue<Task> tasks;           ///< @brief All tasks waiting for a worker.
    };

    std::atomic_bool m_terminate = false; ///< @brief Flag indicating pool shutdown.
    ConditionVariable m_condition;        ///< @brief Used to wake up workers for a new task.
    Mutex<Inner> m_inner;                 ///< @brief Internal data locked behind a @ref Mutex.
};

} // namespace siren::core
