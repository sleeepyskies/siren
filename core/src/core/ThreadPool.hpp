#pragma once

#include <condition_variable>
#include <functional>
#include <shared_mutex>
#include <thread>
#include "utilities/spch.hpp"


namespace siren::core
{
class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(u32 thread_count = std::thread::hardware_concurrency() - 1); // -1 for main thread
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    template <typename Fn>
        requires(std::is_invocable_v<Fn>)
    void enqueue(Fn&& task)
    {
        std::unique_lock lock(m_mutex);
        m_tasks.push(std::move(task));
        lock.unlock(); // unlock before notifying so the thread doesn't have to wait
        m_cv.notify_one();
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
