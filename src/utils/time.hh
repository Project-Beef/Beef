#ifndef __UTILS__TIME__HH__
#define __UTILS__TIME__HH__

#include <chrono>
#include <string>

#include <fmt/format.h>
#include <fmt/chrono.h>

namespace beef
{
    using clock = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;

    namespace time
    {
        std::string to_string(const std::chrono::seconds& time) noexcept;
    }
}

#endif