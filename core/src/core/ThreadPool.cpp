#include "ThreadPool.hpp"


namespace siren::core
{
ThreadPool::ThreadPool(const u32 thread_count)
{
    for (i32 i = 0; i < thread_count; i++) {
        m_threads.emplace_back(std::thread{ &ThreadPool::loop, this });
    }
}

ThreadPool::~ThreadPool()
{
    std::unique_lock lock(m_mutex);
    m_terminate = true; // lock since threads read this flag
    lock.unlock();

    // wake all threads, then wait for them to finish any pending tasks
    m_cv.notify_all();
    for (auto& thread : m_threads) { thread.join(); }
    m_threads.clear();
}

void ThreadPool::loop()
{
    while (true) {
        std::unique_lock lock(m_mutex);
        // sleep the thread, once awoken, it will check if p is true, and only then continue
        m_cv.wait(lock, [this]() { return m_terminate || !m_tasks.empty(); });
        if (m_terminate && m_tasks.empty()) { return; }
        Task task = std::move(m_tasks.front());
        m_tasks.pop();
        lock.unlock();
        task();
    }
}
}
