#include "render_thread.hpp"

#include <GLFW/glfw3.h>

#include "window/WindowModule.hpp"


namespace siren::core
{

RenderThread::RenderThread() {
    #ifndef SIREN_SINGLE_THREADED
    auto inner    = m_inner.lock();
    inner->thread = std::thread{ &RenderThread::run, this };
    #endif
}

RenderThread::~RenderThread() { { }
    m_terminate = true;
    m_condition.notify_all();

    std::thread render_thread;
    m_inner.run_scoped(
        [&] (UniqueGuard<Inner>& inner) {
            render_thread = std::move(inner->thread);
        }
    );

    if (render_thread.joinable()) {
        render_thread.join();
    }
}

void RenderThread::spawn(RenderTask&& task) {
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

auto RenderThread::run() -> void {
    // todo: make backend agnostic
    glfwMakeContextCurrent((GLFWwindow*)Locator<WindowModule>::locate().handle());

    while (true) {
        std::queue<RenderTask> local_tasks;

        m_inner.run_scoped(
            [&] (UniqueGuard<Inner>& inner) {
                m_condition.wait_while(
                    inner,
                    [&inner, this] {
                        return m_terminate || !inner->tasks.empty();
                    }
                );
                if (m_terminate && inner->tasks.empty()) { return; }

                // grab all tasks since is only one thread anyway and avoid multiple locks then
                std::swap(local_tasks, inner->tasks);
            }
        );

        while (!local_tasks.empty()) {
            local_tasks.front()();
            local_tasks.pop();
        }
    }
}

} // namespace siren::core
