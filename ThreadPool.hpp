#pragma once

#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <chrono>
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <queue>

namespace Can
{
    class ThreadPool
    {
    private:
        void ThreadLoop();
        std::mutex queue_mutex;                  // Prevents data races to the job queue
        std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> jobs;
        uint16_t pool_size;            // Number of threads to be
        bool should_terminate = false; // Tells threads to stop looking for jobs

    public:
        void Start();
        void QueueJob(const std::function<void()> &job);
        void Stop();
    };
}