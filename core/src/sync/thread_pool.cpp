#include "thread_pool.hpp"


namespace siren::core
{
ThreadPool::ThreadPool(const u32 thread_count) {
    #ifndef SIREN_SINGLE_THREADED
    auto inner = m_data.lock();
    for (i32 i = 0; i < thread_count; i++) {
        inner->threads.emplace_back(std::thread{ &ThreadPool::loop, this });
    }
    #endif
}

ThreadPool::~ThreadPool() {
    auto inner       = m_data.lock();
    inner->terminate = true; // lock since threads read this flag

    // wake all threads, then wait for them to finish any pending tasks
    m_cv.notify_all();
    for (auto& thread : inner->threads) { thread.join(); }
    inner->threads.clear();
}

void ThreadPool::spawn(Task&& task) {
#ifdef SIREN_SINGLE_THREADED
    task();
#else
    {
        // unlock before notifying so the thread doesn't have to wait
        auto inner = m_data.lock();
        inner->tasks.push(std::move(task));
    }
    m_cv.notify_one();
#endif
}

void ThreadPool::loop() {
    while (true) {
        Task task;

        {
            auto inner = m_data.lock();
            m_cv.wait_while(
                inner,
                [&inner] {
                    return inner->terminate || !inner->tasks.empty();
                }
            );
            if (inner->terminate && inner->tasks.empty()) { return; }
            task = std::move(inner->tasks.front());
            inner->tasks.pop();
        }

        // avoids stalling other threads while doing something heavy
        if (task) {
            task();
        }
    }
}
} // namespace siren::core
