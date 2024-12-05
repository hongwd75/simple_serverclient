#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>
#include <sstream>
#include "ClientManager.h"

class TaskQueueThread
{
public:
    TaskQueueThread();
    ~TaskQueueThread();

    void start();
    void stop();

    void addTask(TaggedTask task);
    void addTask(int id,TaggedTask task);

private:
    void distributeTask(TaggedTask task);
    void workerThread(int threadId);

private:
    std::atomic<bool> running;
    std::atomic<int> nextQueue;
    int numThreads;
    std::vector<std::thread> threadPool;
    std::vector<std::queue<TaggedTask>> taskQueues;
    std::vector<std::unique_ptr<std::mutex>> queueMutexes;
    std::vector<std::unique_ptr<std::condition_variable>> queueCvs;
};

