#include <memory>  // For std::shared_ptr
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include "Task.h"


class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<Task>> tasks;  // Queue holding shared_ptr<Task>
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;

public:
    ThreadPool(int numThreads);
    ~ThreadPool();
    void enqueue(std::shared_ptr<Task> task);  // Accept shared_ptr<Task> in enqueue
    void workerLoop();
};
