module;

#include <functional>
#include <atomic>
#include <thread>
#include <queue>
#include <GLFW/glfw3.h>

export module siren.render.render_thread;

import siren.sync;

namespace siren::render {

/**
 * @class RenderThread
 * @brief Manages the execution of all GPU operations and graphics API calls.
 */
export class RenderThread {
public:
    /** @brief A command to be computed on the RenderThread. */
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
    /** @brief Main worker loop. */
    auto run() -> void;

    /** @brief Holds inner data. */
    struct Inner {
        /** @brief The queue of @ref RenderTask's to perform. */
        std::queue<RenderTask> tasks;
        /** @brief The actual worker thread. */
        std::thread thread;
    };

    /** @brief Flag indicating if the thread should terminate. */
    std::atomic_bool m_terminate;
    /** @brief Number of tasks left to process. A task being process still counts towards this value. */
    std::atomic<usize> m_task_count;
    /** @brief Used to sleep and wakeup the thread. */
    sync::ConditionVariable m_condition;
    /** @brief Locked inner data. */
    sync::Mutex<Inner> m_inner;
};

RenderThread::RenderThread() {
    if constexpr (!SINGLE_THREADED) {
        auto inner    = m_inner.lock();
        inner->thread = std::thread{ &RenderThread::run, this };
    }
}

RenderThread::~RenderThread() { { }
    m_terminate = true;
    m_condition.notify_all();

    std::thread render_thread;
    m_inner.run_scoped(
        [&] (sync::UniqueGuard<Inner>& inner) -> std::thread {
            std::move(inner->thread);
        }
    );

    if (render_thread.joinable()) {
        render_thread.join();
    }
}

auto RenderThread::spawn(RenderTask&& task) -> void {
    if constexpr (SINGLE_THREADED) {
        task();
    } else {
        m_task_count.fetch_add(1);
        // unlock before notifying so the thread doesn't have to wait
        m_inner.run_scoped(
            [&] (sync::UniqueGuard<Inner>& inner) {
                inner->tasks.push(std::move(task));
            }
        );
        m_condition.notify_one();
    }
}

auto RenderThread::wait_until_idle() const noexcept -> void {
    usize remaining = m_task_count.load();

    while (remaining > 0) {
        m_task_count.wait(remaining);
        remaining = m_task_count.load();
    }
}

auto RenderThread::run() -> void {
    glfwMakeContextCurrent((GLFWwindow*)Locator<Window>::value().handle());

    while (true) {
        std::queue<RenderTask> local_tasks;

        m_inner.run_scoped(
            [&] (sync::UniqueGuard<Inner>& inner) {
                m_condition.wait(
                    inner, [&inner, this] {
                        return m_terminate || !inner->tasks.empty();
                    }
                );
                if (m_terminate && inner->tasks.empty()) { return; }

                // grab all tasks since is only one thread anyway and avoid multiple locks then
                std::swap(local_tasks, inner->tasks);
            }
        );

        while (!local_tasks.empty()) {
            local_tasks.front()(); // <-- we call the fn here to incase u didn't see ()()
            local_tasks.pop();
            m_task_count.fetch_sub(1);
        }
    }
}

} // namespace siren::render
