#include "TaskQueueThread.h"

TaskQueueThread::TaskQueueThread() : running(true), nextQueue(0) {
    numThreads = std::thread::hardware_concurrency();
    taskQueues.resize(1000);
    for (size_t i = 0; i < numThreads; ++i) {
        queueMutexes.push_back(std::make_unique<std::mutex>());
        queueCvs.push_back(std::make_unique<std::condition_variable>());
    }
}

TaskQueueThread::~TaskQueueThread()
{
    stop();
}

void TaskQueueThread::start()
{
    for (int i = 0; i < numThreads; ++i) {
        threadPool.emplace_back(&TaskQueueThread::workerThread, this, i);
    }
}

void TaskQueueThread::stop()
{
    running = false;
    for (auto& cv : queueCvs) {
        cv->notify_all();
    }
    for (auto& thread : threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void TaskQueueThread::addTask(TaggedTask task)
{
    distributeTask(std::move(task));
}
void TaskQueueThread::addTask(int id, TaggedTask task)
{
    std::unique_lock<std::mutex> lock(*queueMutexes[id]);
    taskQueues[id].push({ std::move(task) });
    queueCvs[id]->notify_one();
}

void TaskQueueThread::distributeTask(TaggedTask task)
{
    int queueIndex = nextQueue++ % numThreads;
    std::unique_lock<std::mutex> lock(*queueMutexes[queueIndex]);
    taskQueues[queueIndex].push({ std::move(task) });
    queueCvs[queueIndex]->notify_one();
}

void TaskQueueThread::workerThread(int threadId)
{
    while (running) {
        TaggedTask taggedTask;
        int count = 0;
        {
            std::unique_lock<std::mutex> lock(*queueMutexes[threadId]);
            queueCvs[threadId]->wait(lock, [this, threadId] {
                return !taskQueues[threadId].empty() || !running;
                });

            if (!running && taskQueues[threadId].empty()) {
                break;
            }

            taggedTask = std::move(taskQueues[threadId].front());
            taskQueues[threadId].pop();
            count = taskQueues[threadId].size();
        }

        if (taggedTask.task) {
            auto start = std::chrono::high_resolution_clock::now();
            taggedTask.task();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            std::ostringstream oss;
            oss << "쓰레드 " << threadId << " 작업 실행 시간: " << duration.count() << "ms  | 남은수 :" << count <<"\n";
            OutputDebugStringA(oss.str().c_str());
        }
    }
}
