#include "text.hh"
#include <algorithm>

namespace beef::text
{
    bool get_str_from_packet(ENetPacket* packet, std::string& str)
    {
        packet->data[packet->dataLength - 1] = 0;
        str = reinterpret_cast<char*>(packet->data + 4);
        
        return !(packet->dataLength < 5 || packet->dataLength > 512);
    }


    namespace proton
    {
        std::string cypher(const std::string& input, uint32_t id)
        {
            if (input.empty()) return "";
            constexpr std::string_view secret = "PBG892FXX982ABC*";

            std::string return_value(input.size(), 0);

            for (uint32_t i = 0; i < input.size(); ++i)
                return_value[i] = input[i] ^ secret[(i + id) % secret.size()];

            return return_value;
        }

        uint32_t hash(const void* data, uintmax_t data_len)
        {
            if (!data) return 0;
            uint32_t hash = 0x55555555;

            for (uintmax_t i = 0; i < data_len; ++i)
                hash = (hash >> 27) + (hash << 5) + static_cast<const uint8_t*>(data)[i];

            return hash;
        }
    }


    bool to_uppercase(std::string& str)
    {
        bool success = true;
        if (str.empty()) return false;
        
        for (auto& ch : str)
        {
            if (
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z')
            )
                ch = toupper(ch);
                
            else if (!(ch >= '0' && ch <= '9'))
            {
                success = false;
                break;
            }
        }

        return success;
    }

    bool to_lowercase(std::string& str)
    {
        bool success = true;
        if (str.empty()) return false;
        
        for (auto& ch : str)
        {
            if (
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z')
            )
                ch = tolower(ch);
                
            else if (!(ch >= '0' && ch <= '9'))
            {
                success = false;
                break;
            }
        }

        return success;
    }

    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }
}
