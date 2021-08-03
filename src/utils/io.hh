#ifndef __UTILS__IO__HH__
#define __UTILS__IO__HH__

#include <stdint.h>
#include <string_view>

namespace beef::io
{
    void* read_all_bytes(const std::string_view& filename, uintmax_t& filesize) noexcept;
    void write_all_bytes(const std::string_view& filename, const void* data, uintmax_t data_size) noexcept;

    bool create_symlink(const char* linkName, const char* file) noexcept;
    bool create_link(const char* linkName, const char* file) noexcept;
}

#endif // __UTILS__IO__HH__
