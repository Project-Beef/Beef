#include "scanner.hh"

#include <chrono>
#include <string>
#include <exception>
#include <stdexcept>
#include <unordered_map>

namespace beef::text
{
    scanner::scanner()
    {
    }

    scanner::scanner(const std::string& data)
    {
        std::string::size_type key_pos = 0;
        std::string::size_type i = 0;

        std::string key, val;

        while (i != std::string::npos)
        {
            if (i + 2 >= data.size()) break;

            key_pos = data.find('|', i + 2);
            if (key_pos == std::string::npos)
                break;

            key = data.substr(
                (i == 0 || i == 1 ? i : i + 1), 
                key_pos - i - (i == 0 || i == 1 ? 0 : 1)
            );

            if (i + 1 >= data.size()) break;
            
            i = data.find('\n', i + 1);
            if (i == std::string::npos)
                val = data.substr(key_pos + 1);
            else
                val = data.substr(key_pos + 1, i - key_pos - 1);

            m_data.insert_or_assign(
                std::move(key),
                std::move(val)
            );
        }
    }

    bool scanner::try_get(const std::string& key, std::string& value) noexcept
    {
        if (auto it = m_data.find(key); it != m_data.end())
        {
            value = it->second;
            return true;
        }

        return false;
    }

    bool scanner::try_get(const std::string& key, int& value) noexcept
    {
        if (auto it = m_data.find(key); it != m_data.end())
        {
            value = atoi(it->second.c_str());
            return true;
        }

        return false;
    }

    const std::string& scanner::get(const std::string& key)
    {
        return m_data.at(key);
    }

    const std::string& scanner::get_safe(const std::string& key) noexcept
    {
        if (has(key))
            return m_data.at(key);

        return m_data.begin()->second;
    }

    bool scanner::has(const std::string& key)
    {
        if (m_data.find(key) != m_data.end())
            return true;

        return false;
    }
}