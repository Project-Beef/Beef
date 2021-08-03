#ifndef __UTILS__RNG__HH__
#define __UTILS__RNG__HH__

#include <chrono>
#include <random>

namespace beef::rng
{
    template<typename T>
    T gen(T min, T max)
    {
        //static thread_local std::random_device rd;
        //static thread_local std::uniform_int_distribution<T> dist(min, max);
        
        //return dist(rd);

        static std::random_device rd;
        static std::mt19937::result_type seed = rd() ^ (
                (std::mt19937::result_type)
                std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                    ).count() +
                (std::mt19937::result_type)
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
                    ).count());

        static std::mt19937 gen(seed);
        static std::uniform_int_distribution<T> distrib(min, max);

        return distrib(rd);
    }
}

#endif // __UTILS__RNG__HH__
