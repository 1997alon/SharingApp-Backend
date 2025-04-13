#include "ThreadPool.h"
#include "SignUpTask.h"
#include "LoginTask.h"

ThreadPool::ThreadPool(int numThreads) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& thread : workers) {
        thread.join();
    }
}

void ThreadPool::enqueue(std::shared_ptr<Task> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);  // Push shared_ptr<Task> to the queue
    }
    condition.notify_one();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::shared_ptr<Task> task = nullptr;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            task = tasks.front();
            tasks.pop();
        }

        if (task)
            task->run();
    }
}
