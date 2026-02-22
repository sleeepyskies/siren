#include "thread_pool.hpp"


namespace siren::core
{
ThreadPool::ThreadPool(const i32 thread_count) {
#ifndef SIREN_SINGLE_THREADED
    u32 count = glm::max(1, thread_count);
    if (thread_count < 0) {
        count = glm::max(std::thread::hardware_concurrency() - thread_count, 1u);
    }

    log()->info("Creating a ThreadPool with {} threads", count);

    auto inner = m_inner.lock();
    for (i32 i = 0; i < count; i++) {
        inner->threads.emplace_back(std::thread{ &ThreadPool::run, this });
    }
#endif
}

ThreadPool::~ThreadPool() {
    m_terminate = true;

    std::vector<std::thread> threads_to_join;
    m_condition.notify_all();
    {
        auto inner = m_inner.lock();
        for (auto& thread : inner->threads) {
            threads_to_join.push_back(std::move(thread));
        }
    }

    for (auto& thread : threads_to_join) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadPool::spawn(Task&& task) {
#ifdef SIREN_SINGLE_THREADED
    task();
#else
    // unlock before notifying so the thread doesn't have to wait
    m_inner.run_scoped(
        [&] (UniqueGuard<Inner>& inner) {
            inner->tasks.push(std::move(task));
        }
    );
    m_condition.notify_one();
#endif
}

void ThreadPool::run() {
    while (true) {
        Task task;

        m_inner.run_scoped(
            [&] (UniqueGuard<Inner>& inner) {
                m_condition.wait_while(
                    inner,
                    [&inner, this] {
                        return m_terminate || !inner->tasks.empty();
                    }
                );
                if (m_terminate && inner->tasks.empty()) { return; }
                task = std::move(inner->tasks.front());
                inner->tasks.pop();
            }
        );

        // avoids stalling other threads while doing something heavy
        if (task) {
            task();
        }
    }
}
} // namespace siren::core
