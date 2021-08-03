#include "time.hh"

namespace beef::time
{
    std::string to_string(const std::chrono::seconds& time) noexcept
    {
        static int day_count = 24 * 3600;
        static int hour_count = 3600;
        static int min_count = 60;

        int n = time.count();

        int day  = n / day_count;
        n %= day_count;
        int hour = n / hour_count;
        n %= hour_count;
        int min  = n / min_count;
        n %= min_count;
        int sec  = n;

        std::string result;

        if (day != 0)
            result.append(fmt::format("{} days, ", day));
        if (hour != 0)
            result.append(fmt::format("{} hours, ", hour));
        if (min != 0)
            result.append(fmt::format("{} mins, ", min));

        result.append(fmt::format("{} secs", sec));

        return result;
        //return fmt::format("{} days, {} hours, {} mins, {} secs", day, hour, min, sec);
    }
}
