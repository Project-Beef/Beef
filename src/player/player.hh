#ifndef __PLAYER__PLAYER__HH__
#define __PLAYER__PLAYER__HH__

#include <enet/enet.h>
#include <fmt/core.h>

#include <memory>
#include <stdint.h>
#include <string>

#include "clothing.hh"
#include "inventory.hh"
#include "utils/time.hh"
#include "math/vec2i.hh"
#include "text/scanner.hh"
#include "itemdb/itemdb.hh"
#include "packet/variant.hh"
#include "constants/roles.hh"

namespace beef
{
    class server;
    class world_manager;

    class player
    {
    public:
        player(ENetPeer* peer, world_manager* manager);

        void set_net_id(int id);
        void set_user_id(int id);

        int get_net_id() const;
        int get_user_id() const;

        void send_packet(const void* data, uintmax_t data_size, uint32_t flags = 0);
        void send_packet(uint32_t type, const void* data = nullptr, uintmax_t data_size = 0, uint32_t flags = 0);

        void send_hello();
        void send_text(const std::string_view& data, uint32_t flags = ENET_PACKET_FLAG_RELIABLE);
        void send_log(const std::string_view& str);

        void send_variant(const variantlist& vlist, int net_id = -1, int delay = -1, uint32_t flags = ENET_PACKET_FLAG_RELIABLE);
        void send_main();
        void send_cloth(const player* player_, bool sound = false);

        void send_inventory();

        void set_peer_data();
        void disconnect_peer();

        bool get_info(text::scanner* scanner);
        bool get_info_tank(text::scanner* scanner);

        void ban(const std::chrono::seconds& time, server* server_);

        bool has_inventory_item(uint16_t item);
        bool add_inventory_item(uint16_t item, uint8_t count = 200, bool send = false);
        void unequip_cloth(const uint16_t& item);
        bool remove_inventory_item(uint16_t item, uint8_t count = 200, bool send = false);
        uint8_t get_inventory_item_count(uint16_t item); // not safe! call this when u have check if the item exists!

        void set_display_name_and_chat_color();
        std::string get_display_name();

    private:
        typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>::type> days;
        void update_inventory_item(uint16_t item, uint8_t count = 200, bool adding = false);

    public:
        std::string m_current_world = "EXIT";

        std::string m_display_name = "";
        std::string m_tank_id = "";
        std::string m_tank_pass = "";
        std::string m_requested_name = "";

        std::string m_raw_name = "";
        std::string m_raw_pass = "";

        int m_protocol = 0;
        int m_platform = 0;
        int m_device_version = 0;

        int m_version = 0;
        std::string m_version_str = "";
        
        std::string m_rid = "";
        std::string m_mac = "";
        std::string m_country = "us";

        clothing m_cloth;
        inventory m_inv;

        int m_gems = 0;
        int m_admin_level = LVL_NORMAL;
        int m_skin_color = 0x8295C3FF;

        char m_chat_color = 'w';

        bool m_rotated_left = false;
        bool m_logged_in = false;
        bool m_guest = true;

        std::string m_ip = "";
        std::string m_raw_ip = "";

        std::string m_email = "";
        std::string m_discord = "";
        std::string m_ban_reason = ""; 

        time_t m_created_at = 0;
        time_point m_banned_at = clock::now();
        std::chrono::seconds m_banned_time = {};

        math::vec2i m_pos = { 0, 0 };
        

    private:
        ENetPeer* m_peer;
        world_manager* m_world_manager;

        int m_net_id;
        int m_user_id;
    };

    /*
    class player
    {
    public:
        player(ENetPeer* peer, std::shared_ptr<beef::server>& server);

        uint32_t get_net_id() noexcept;
        uint64_t get_user_id() noexcept;

        // void send_raw(uint32_t type, void* data = nullptr, size_t len = 0, uint32_t flags = 0);
        void send_raw(uint32_t type, const void* data = nullptr, size_t len = 0, uint32_t flags = 0);

        // same as send_raw, but type has to be embedded into data
        void send_raw(const void* data, size_t len = 0, uint32_t flags = 0);

        void send_hello();
        void send_variant(const variantlist& vlist, uint32_t net_id = -1, uint32_t delay = -1, uint32_t flags = ENET_PACKET_FLAG_RELIABLE);
        void send_text(const std::string& data, uint32_t flags = ENET_PACKET_FLAG_RELIABLE);
        void send_log(const std::string& str);

        void send_variant_onsupermain(uint32_t flags = ENET_PACKET_FLAG_RELIABLE);

        void set_peer_data();
        ENetPeer* get_peer();

        const player_data& get_data();
        player_data& get_data_raw();

        const player_inventory& get_inventory();
        void add_inventory_item(uint32_t item, uint32_t amount = 200);
        void remove_inventory_item(uint32_t item, uint32_t amount = 200);
        void set_inventory_size(uint32_t size);
        void send_inventory();

        void set_guest_data(beef::text::scanner& scanner);
        void set_regist_data(beef::text::scanner& scanner);

        void set_net_id(uint32_t net_id);
        void set_user_id(uint64_t user_id);

        //void save_clothing(std::shared_ptr<pqxx::connection>& connection);

        //[[deprecated]]
        //void save_inventory(std::shared_ptr<pqxx::connection>& connection);

        const std::string& get_ip() noexcept;

        void disconnect();

    private:
        uint32_t m_net_id;
        uint64_t m_user_id;

        ENetPeer* m_peer;
        std::shared_ptr<beef::server> m_server;

        std::string m_ip;
        player_data m_data;
        player_inventory m_inventory;
    };*/
}

#endif // __PLAYER__PLAYER__HH__