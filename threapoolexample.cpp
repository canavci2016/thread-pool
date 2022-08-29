#include <iostream>
#include <thread>
#include "ThreadPool.hpp"

int main(int argc, char **argv)
{

    uint32_t thread_count = std::thread::hardware_concurrency();
    std::cout << "supported thread count by os: " << thread_count << std::endl;
    
    Can::ThreadPool pool(thread_count);
    pool.Start();

    char c;
    while (1)
    {
        std::cout << "in order to add value to queue write 'a' and to press 'e' : " << std::endl;
        std::cin >> c;

        if (c == 'a')
        {
            pool.QueueJob([]()
                          { std::cout << "a is written on terminal" << std::endl; });
        }
    }
}
