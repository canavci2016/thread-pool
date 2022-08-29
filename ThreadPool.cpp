#include "ThreadPool.hpp"

// -------- public methods ----------------

/*
once threads are created according to num_threads.
it's better not to create new ones or destroy old ones
There will be a performance penalty. it might even make your application go slower than the serial version
Thus, we keep a pool of threads that can be used at any time
each thread should be running its own infinite loop, constantly waiting for new tasks to grab and run.

*/

Can::ThreadPool::ThreadPool(uint16_t pool_size)
{
    this->pool_size = pool_size;
}

void Can::ThreadPool::Start()
{
    const uint16_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    threads.resize(num_threads);
    for (uint16_t i = 0; i < num_threads; i++)
    {
        // threads.push_back(std::thread(&Can::ThreadPool::ThreadLoop, this));
        threads.push_back(std::thread([this]()
                                      { ThreadLoop(); }));
        /*threads.at(i) = std::thread([this]()
                                    { ThreadLoop(); });*/
    }
}

/*
The infinite loop function. This is a while (true) loop waiting for the task queue to open up.

*/
void Can::ThreadPool::ThreadLoop()
{
    while (true)
    {
        std::function<void()> job;

        std::unique_lock<std::mutex> lock(queue_mutex);
        std::cout << std::this_thread::get_id() << " thread is waiting for queue to open up" << std::endl;
        mutex_condition.wait(lock, [this]()
                             { return !jobs.empty() || should_terminate; });

        if (should_terminate)
        {
            std::cout << std::this_thread::get_id() << " thread is terminated" << std::endl;
            return;
        }

        std::cout << std::this_thread::get_id() << " thread is now opened to run" << std::endl;

        job = jobs.front();
        jobs.pop();
        job();
    }
}

void Can::ThreadPool::QueueJob(const std::function<void()> &job)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

void Can::ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}

// -------- private methods ----------------
