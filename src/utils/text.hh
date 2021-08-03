#ifndef __UTILS__TEXT__HH__
#define __UTILS__TEXT__HH__

#include <string>
#include <algorithm>

#include <enet/enet.h>

namespace beef::text
{
    bool get_str_from_packet(ENetPacket* packet, std::string& str);

    namespace proton
    {
        std::string cypher(const std::string& input, uint32_t id);
        uint32_t hash(const void* data, uintmax_t data_len);
    }

    bool to_uppercase(std::string& str);
    bool to_lowercase(std::string& str);

    void ltrim(std::string& str);
    void rtrim(std::string& str);
    void trim(std::string& str);

    constexpr uint64_t quick_hash(const std::string_view& input)
    {
        if (input.empty()) return 0;
        uint64_t hash = 0;

        for (size_t i = 0; i < input.size(); ++i)
        {
            hash += input[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        hash ^= (hash >> 23);
        hash += (hash << 27);

        return hash;
    }

    constexpr uint64_t operator "" _qh(const char* str, size_t len)
    {
        return quick_hash(std::string_view(str, len));
    }
}

#endif // __UTILS__TEXT__HH__
