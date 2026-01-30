#pragma once

#include <condition_variable>
#include <thread>

#include "utilities/spch.hpp"
#include "core/Core.hpp"


namespace siren::core
{
class TaskPool {
public:
    using Task = std::function<void()>;

    explicit TaskPool(u32 thread_count = std::thread::hardware_concurrency() - 1); // -1 for main thread
    ~TaskPool();

    TaskPool(const TaskPool&)            = delete;
    TaskPool(TaskPool&&)                 = delete;
    TaskPool& operator=(const TaskPool&) = delete;
    TaskPool& operator=(TaskPool&&)      = delete;

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
    void loop();

    Vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::queue<Task> m_tasks;
    std::condition_variable m_cv; // used to wake and sleep threads
    bool m_terminate = false;
};
}
