module;

#include <vector>
#include <future>
#include <queue>
#include <functional>
#include <atomic>
#include <thread>

export module siren.sync:thread_pool;

import :condition_variable;
import :mutex;

import siren.common;
import siren.log;

export namespace siren::sync {

/// @todo: another pool for more idle background tasks? IOPool or something?

/**
 * @class ThreadPool
 * @brief A worker pool for async task execution.
 * Manages a set of workers that handle incoming tasks.
 * Note that this should not be used for any GPU related
 * tasks, but rather CPU tasks only, @ref see GpuWorker.
 */
class ThreadPool {
    /**
     * @brief Type erased internal task type. Handles calling the
     * provided function with its arguments.
     */
    using Task = std::move_only_function<void()>;

public:
    /**
     * @brief Creates a new threadpool.
     * @param thread_count The number of threads to create.
     * Takes the number of available hardware threads by
     * default. A negative number creates hardware_threads - x
     * threads, with a minimum of 1.
     *
     * @note If SIREN_SINGLE_THREADED is defined, no threads
     * will be created and all spawned tasks will be executed
     * on the main thread.
     */
    explicit ThreadPool(i32 thread_count = std::jthread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    /**
     * @brief Retrieves the singleton instance of this ThreadPool.
     * @warning Crashes if ThreadPool::init() has not been called yet. This is
     * handled by the @ref SyncPlugin.
     */
    static auto get() -> ThreadPool& { return *s_instance.get(); }

    /** @brief Initializes the global singleton instance. */
    static auto init() -> void { s_instance = std::make_unique<ThreadPool>(); }

    /**
     * @brief Runs a provided task asynchronously (if siren::SINGLE_THREADED is false).
     * @note This function returns nothing, so the called must handle results of the function.
     * To receive a future, see ThreadPool::spawn().
     * @tparam Func The function type.
     * @tparam Args The argument types of the function.
     * @param func The function to run.
     * @param args The arguments to provide to the function.
     */
    template <typename Func, typename... Args>
        requires std::is_invocable_v<Func, Args...>
    auto spawn_detached(Func&& func, Args&&... args) -> void {
        Task task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        if constexpr (SINGLE_THREADED) {
            // runs immediately on this thread.
            task();
        } else {
            // unlock before notifying so the thread doesn't have to wait
            m_inner.run_scoped(
                [&task] (UniqueGuard<Inner>& inner) {
                    inner->tasks.push(std::move(task));
                }
            );
            m_condition.notify_one();
        }
    }

    /**
     * @brief Runs the provided function in the background and returns a future to
     * read its value.
     * @tparam Func The function type.
     * @tparam Args The argument types of the function.
     * @param func The function to run.
     * @param args The arguments to provide to the function.
     * @return A future holding the return value of the asynchronous task.
     */
    template <typename Func, typename... Args>
        requires(std::is_invocable_v<Func, Args...>)
    [[nodiscard]]
    auto spawn(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
        using ReturnType = std::invoke_result_t<Func, Args...>;

        std::packaged_task<ReturnType()> packaged_task{
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        };

        auto future = packaged_task.get_future();

        if constexpr (SINGLE_THREADED) {
            packaged_task();
        } else {
            // unlock before notifying so the thread doesn't have to wait
            m_inner.run_scoped(
                [packaged_task = std::move(packaged_task)] (UniqueGuard<Inner>& inner) mutable {
                    inner->tasks.push([t = std::move(packaged_task)] mutable { t(); });
                }
            );
            m_condition.notify_one();
        }

        return future;
    }

private:
    friend class SyncPlugin;

    /** @brief Main worker loop for a thread. */
    void run();

    /** @brief Internal data of ThreadPool. */
    struct Inner {
        std::vector<std::jthread> threads; ///< @brief The pool of threads.
        std::queue<Task> tasks;            ///< @brief All tasks waiting for a worker.
    };

    std::atomic_bool m_terminate = false; ///< @brief Flag indicating pool shutdown.
    ConditionVariable m_condition;        ///< @brief Used to wake up workers for a new task.
    Mutex<Inner> m_inner;                 ///< @brief Internal data locked behind a @ref Mutex.

    /** @brief The single static instance. */
    static inline std::unique_ptr<ThreadPool> s_instance;
};

ThreadPool::ThreadPool(const i32 thread_count) {
    if constexpr (!SINGLE_THREADED) {
        u32 count = std::max(1, thread_count);
        if (thread_count < 0) {
            count = std::max(std::jthread::hardware_concurrency() + thread_count, 1u);
        }

        log::info("New ThreadPool initialized with {} threads", count);

        auto inner = m_inner.lock();
        for (i32 i = 0; i < count; i++) {
            inner->threads.emplace_back(std::jthread{ &ThreadPool::run, this });
        }
    }
}

ThreadPool::~ThreadPool() {
    m_terminate.store(true);
    m_condition.notify_all();
}

auto ThreadPool::run() -> void {
    Task task;

    while (true) {
        task = m_inner.run_scoped(
            [&] (auto& inner) -> Task {
                m_condition.wait(
                    inner,
                    [&inner, this] {
                        return m_terminate || !inner->tasks.empty();
                    }
                );
                if (m_terminate && inner->tasks.empty()) {
                    return nullptr;
                }
                task = std::move(inner->tasks.front());
                inner->tasks.pop();
                return std::move(task);
            }
        );

        if (!task && m_terminate) {
            break;
        }

        // avoids stalling other threads while doing something heavy
        if (task) {
            task();
        }
    }
}
} // namespace siren::sync
