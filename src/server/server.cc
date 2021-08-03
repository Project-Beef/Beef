#include "server.hh"

#include <chrono>
#include <stdexcept>
#include <exception>


namespace beef
{
    server::server(size_t peers, uint16_t port)
        : m_address({ ENET_HOST_ANY, port })
    {
        m_running.store(true);

        m_host = enet_host_create(&m_address, peers, 2, 0, 0);
        if (!m_host)
        {
            fmt::print("failed to initialize enet!\n");
            exit(-6);
        }

        m_host->checksum = enet_crc32;
        enet_host_compress_with_range_coder(m_host);

        m_clock = std::chrono::system_clock::now();
    }

    server::~server()
    {
    }

    int server::service(ENetEvent& event, uint32_t timeout)
    {
        return enet_host_service(m_host, &event, timeout);
    }

    ENetHost* server::get_host_ptr() noexcept
    {
        return m_host;
    }

    ENetAddress server::get_address() noexcept
    {
        return m_address;
    }

    bool server::is_running() const noexcept
    {
        return m_running.load();
    }

    void server::set_running(bool status) noexcept
    {
        m_running.store(status);
    }

    const std::chrono::time_point<std::chrono::system_clock>& server::get_uptime() const
    {
        return m_clock;
    }

    int server::get_user_id() noexcept
    {
        return m_uid++;
    }

    void server::set_user_id(int id) noexcept
    {
        m_uid = id;
    }


    player* server::get_player_by_name(const std::string& raw_name)
    {
        if (auto it = m_players_by_name.find(raw_name); it != m_players_by_name.end())
            return it->second;

        return nullptr;
    }

    player* server::get_player_by_uid(int uid)
    {
        if (auto it = m_players_by_uid.find(uid); it != m_players_by_uid.end())
            return it->second;

        return nullptr;
    }


    void server::add_player(player* player_)
    {
        if (player_->m_raw_name.empty() || player_->get_user_id() == -1) return;

        m_players_by_name.insert_or_assign(player_->m_raw_name, player_);
        m_players_by_uid.insert_or_assign(player_->get_user_id(), player_);
    }

    void server::broadcast(const std::function<void(int, player*)>& fn)
    {
        for (auto& player : m_players_by_uid)
        {
            fn(player.first, player.second);
        }
    }
}