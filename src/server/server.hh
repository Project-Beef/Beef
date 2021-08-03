#ifndef __SERVER__SERVER__HH__
#define __SERVER__SERVER__HH__

#include <chrono>
#include <memory>
#include <atomic>
#include <functional>
#include <unordered_map>

#include <stdint.h>

#include "player/player.hh"

#include <enet/enet.h>
#include <fmt/core.h>



namespace beef
{
    enum
    {
        TEXT_EVENTS,
        TANK_EVENTS,
        ACTIONS
    };

    class server
    {
    public:
        server(size_t peers = 1024, uint16_t port = 17093);
        ~server();

        int service(ENetEvent& event, uint32_t timeout = 0);
        
        ENetHost* get_host_ptr() noexcept;
        ENetAddress get_address() noexcept;

        bool is_running() const noexcept;
        void set_running(bool status = false) noexcept;

        const std::chrono::time_point<std::chrono::system_clock>& get_uptime() const;

        int get_user_id() noexcept;
        void set_user_id(int id) noexcept;

        player* get_player_by_name(const std::string& raw_name);
        player* get_player_by_uid(int uid);

        void add_player(player* player_);
        void broadcast(const std::function<void(int, player*)>& fn);

    private:
        ENetHost* m_host;
        ENetAddress m_address;

        std::atomic<bool> m_running;
        std::chrono::time_point<std::chrono::system_clock> m_clock;

        int m_uid;

        std::unordered_map<int, player*> m_players_by_uid;
        std::unordered_map<std::string, player*> m_players_by_name;
    };
}

#endif // __SERVER__SERVER__HH__