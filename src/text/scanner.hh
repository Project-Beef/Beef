#ifndef __UTILS__TEXT_SCANNER__HH__
#define __UTILS__TEXT_SCANNER__HH__

#include <string>
#include <unordered_map>

#include <fmt/core.h>

namespace beef::text
{
    class scanner
    {
    public:
        scanner();
        scanner(const std::string& data);

        bool try_get(const std::string& key, std::string& value) noexcept;
        bool try_get(const std::string& key, int& value) noexcept;

        [[deprecated("Use try_get(const std::string&, std::string&) instead.")]]
        const std::string& get(const std::string& key);
        [[deprecated("Use try_get(const std::string&, std::string&) instead.")]]
        const std::string& get_safe(const std::string& key) noexcept;

        bool has(const std::string& key);

    private:
        std::unordered_map<std::string, std::string> m_data;
    };
}

#endif // __UTILS__TEXT_SCANNER__HH__