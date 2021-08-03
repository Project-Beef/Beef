#include "player.hh"

#include "constants/roles.hh"
#include "constants/config.hh" 

#include "world/world_manager.hh"

#include "server/server.hh"
#include "packet/gameupdatepacket.hh"

#include <chrono>
#include <enet/enet.h>

namespace beef
{
    player::player(ENetPeer* peer, world_manager* manager)
        : m_peer(peer), m_net_id(-1), m_user_id(-1), m_pos(0, 0)
    {
        if (peer)
        {
            set_peer_data();

            m_ip.reserve(16);
            enet_address_get_host_ip(&m_peer->address, m_ip.data(), 16);
        }

        m_world_manager = manager;
    }


    void player::set_net_id(int id)
    {
        m_net_id = id;
    }

    void player::set_user_id(int id)
    {
        m_user_id = id;
    }


    int player::get_net_id() const
    {
        return m_net_id;
    }

    int player::get_user_id() const
    {
        return m_user_id;
    }


    void player::send_packet(const void* data, uintmax_t data_size, uint32_t flags)
    {
        ENetPacket* packet = enet_packet_create(data, data_size, flags);
        if (!packet) 
            return;

        if (enet_peer_send(m_peer, 0, packet) != 0)
            enet_packet_destroy(packet);
    }

    void player::send_packet(uint32_t type, const void* data, uintmax_t data_size, uint32_t flags)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 5 + data_size, flags);
        if (!packet)
            return;

        memcpy(packet->data, &type, 4);
        packet->data[data_size + 4] = 0;
        
        if (data)
            memcpy(packet->data + 4, data, data_size);

        if (enet_peer_send(m_peer, 0, packet) != 0)
            enet_packet_destroy(packet);
    }

    void player::ban(const std::chrono::seconds& time, server* server_)
    {
        m_banned_at = clock::now();
        m_banned_time = time;
        gameupdatepacket* message = variantlist{ "OnConsoleMessage", fmt::format("`#** ```$The Ancient Ones`` have `4banned`` {} `#**`` (`4/rules`` to see the rules!)", m_display_name) }.pack();

        server_->broadcast([&](int, player* p) { p->send_packet(NET_MESSAGE_GAME_PACKET, message, sizeof(gameupdatepacket) + message->m_data_size, ENET_PACKET_FLAG_RELIABLE); });

        std::string time_message = time::to_string(time);

        send_variant(variantlist{ "OnConsoleMessage", fmt::format("`oReality flickers as you begin to wake up. (`$Ban`o mod added, `${}`o left)", time_message) });
        send_variant(variantlist{ "OnConsoleMessage", fmt::format("`#**`$ The Gods`o have used `#Ban `oon {}`o!`# **", m_display_name) });
        
        std::string popup = fmt::format("`wWarning from `4System`w: You've been `4BANNED `wfrom `1Beef`` for {}", time_message);
        
        send_variant(variantlist{ "OnConsoleMessage", popup });
        send_variant(variantlist{ "OnAddNotification", "interface/atomic_button.rttex", popup, "audio/hub_open.wav" });
        disconnect_peer();
    }

    void player::send_hello()
    {
        send_packet(1, nullptr, 0, ENET_PACKET_FLAG_RELIABLE);
    }

    void player::send_text(const std::string_view& data, uint32_t flags)
    {
        send_packet(3, data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    }

    void player::send_log(const std::string_view& str)
    {
        send_text(fmt::format("action|log\nmsg|{}", str));
    }


    void player::send_variant(const variantlist& vlist, int net_id, int delay, uint32_t flags)
    {
        gameupdatepacket* packet = vlist.pack();

        packet->m_net_id = net_id;
        packet->m_delay = delay;

        send_packet(4, packet, packet->m_data_size + sizeof(gameupdatepacket), flags);
        free(packet);
    }

    void player::send_main()
    {
        static std::string p2 = std::string(config::cdn::server_address);
        static std::string p3 = std::string(config::cdn::server_path);
        static std::string p4 = std::string(config::variant::onsupermain::param4);
        static std::string p5 = std::string(config::variant::onsupermain::param5);


        send_variant(variantlist {
            "OnSuperMainStartAcceptLogonHrdxs47254722215a",
            itemdb::get_hash(),
            p2, p3, p4, p5
        });
    }

    void player::send_cloth(const player* player_, bool sound)
    {
        send_variant(variantlist {
                "OnSetClothing",
                math::vec3(player_->m_cloth.m_hair, player_->m_cloth.m_shirt, player_->m_cloth.m_pants),
                math::vec3(player_->m_cloth.m_feet, player_->m_cloth.m_face, player_->m_cloth.m_hand),
                math::vec3(player_->m_cloth.m_back, player_->m_cloth.m_mask, player_->m_cloth.m_necklace),
                player_->m_skin_color,
                math::vec3(player_->m_cloth.m_ances, sound, 0)
        }, player_->get_net_id());
    }


    void player::send_inventory()
    {
        size_t alloc = sizeof(gameupdatepacket) + (4 * m_inv.m_items.size()) + 7;
        gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(alloc));

        if (!packet)
        {
            fmt::print("sending inventory failed\n");
            return;
        }

        memset(packet, 0, alloc);

        packet->m_type = PACKET_SEND_INVENTORY_STATE;
        packet->m_flags |= 8;

        // header or something?
        packet->m_data = 1;
        
        uintmax_t pos = 1;
        memcpy(&packet->m_data + pos, &m_inv.m_size, 4);
        pos += 4;

        uint16_t items = m_inv.m_items.size();
        memcpy(&packet->m_data + pos, &items, 2);
        pos += 2;

        for (const auto& item : m_inv.m_items)
        {
            memcpy(&packet->m_data + pos, &item.first, 2);
            pos += 2;

            (&packet->m_data)[pos++] = item.second;
            (&packet->m_data)[pos++] = 0; // flags: irrelevant
        }
        
        packet->m_data_size = pos;

        send_packet(4, packet, alloc, ENET_PACKET_FLAG_RELIABLE);
    }


    void player::set_peer_data()
    {
        m_peer->data = this;
    }

    void player::disconnect_peer()
    {
        enet_peer_disconnect_later(m_peer, 0);
    }


    bool player::get_info(text::scanner* scanner)
    {
        if (
            !(
                scanner->try_get("requestedName", m_requested_name) &&
                scanner->try_get("protocol", m_protocol) && scanner->try_get("rid", m_rid) && scanner->try_get("platformID", m_platform) &&
                scanner->try_get("deviceVersion", m_device_version) && scanner->try_get("country", m_country) && scanner->try_get("mac", m_mac) &&
                scanner->try_get("game_version", m_version_str)
            )
        )
        {
            send_log("`4Invalid login data");
            return false;
        }

        if (m_version_str.size() >= 2)
        {
            m_version = m_version_str[0] << 16 | m_version_str[2] << 8;
            if (m_version_str.size() >= 3)
                m_version |= m_version_str[3];
        }
        else
            return false;

        m_guest = true;

        return true;
    }

    bool player::get_info_tank(text::scanner* scanner)
    {
        if (
            !(
                scanner->try_get("tankIDName", m_tank_id) && scanner->try_get("tankIDPass", m_tank_pass)
            )
        )
        {
            send_log("`4Invalid login data");
            return false;
        }

        m_guest = false;

        return true;
    }


    bool player::has_inventory_item(uint16_t item)
    {
        return m_inv.m_items.find(item) != m_inv.m_items.end();
    }

    bool player::add_inventory_item(uint16_t item, uint8_t count, bool send)
    {
        if (auto it = m_inv.m_items.find(item); it != m_inv.m_items.end())
        {
            if ((it->second + count) > 200)
            {
                // it->second = 200;

                // if (send)
                //     update_inventory_item(item, count, true);
                // pls see this free - kevz

                return false;
            }
            else
            {
                it->second += count;
            }
        }
        else
        {
            if (m_inv.m_items.size() >= m_inv.m_size)
                return false;
            
            m_inv.m_items.insert_or_assign(item, count);
        }

        if (send)
            update_inventory_item(item, count, true);

        return true;
    }

    void player::unequip_cloth(const uint16_t& item)
    {
        const beef::item& item_ = itemdb::get_item(item);

        if (item_.m_action_type == ITEMTYPE_CLOTHES)
        {
            if (m_cloth.m_ances == item)
                m_cloth.m_ances = 0;

            if (m_cloth.m_back == item)
                m_cloth.m_back = 0;

            if (m_cloth.m_necklace == item)
                m_cloth.m_necklace = 0;

            if (m_cloth.m_face == item)
                m_cloth.m_face = 0;

            if (m_cloth.m_mask == item)
                m_cloth.m_mask = 0;

            if (m_cloth.m_hand == item)
                m_cloth.m_hand = 0;

            if (m_cloth.m_hair == item)
                m_cloth.m_hair = 0;

            if (m_cloth.m_pants == item)
                m_cloth.m_pants = 0;

            if (m_cloth.m_shirt == item)
                m_cloth.m_shirt = 0;

            if (m_cloth.m_feet == item)
                m_cloth.m_feet = 0;

            if (m_world_manager)
            {
                world* world_;
                if (!m_world_manager->get_world(m_current_world, &world_))
                {
                    send_log("`4Invalid world.``");
                }
                else
                {
                    world_->broadcast([&](int net_id, player* player_) {
                        player_->send_cloth(this, true);
                    });
                }
            }
        }
    }

    bool player::remove_inventory_item(uint16_t item, uint8_t count, bool send)
    {
        if (auto it = m_inv.m_items.find(item); it != m_inv.m_items.end())
        {
            if (count > 200)
            {
                m_inv.m_items.erase(it);
                unequip_cloth(item);
            }
            else if (it->second - count == 0 || it->second > 200)
            {
                m_inv.m_items.erase(it);
                unequip_cloth(item);
            }
            else
            {
                it->second -= count;
            }

            if (send)
                update_inventory_item(item, count, false);

            return true;
        }

        return false;
    }


    uint8_t player::get_inventory_item_count(uint16_t item)
    {
        return m_inv.m_items[item];
    }


    void player::set_display_name_and_chat_color()
    {
        char name_color = 'w';

        switch (m_admin_level)
        {
            case LVL_BETA_TESTER:
                name_color = '9';
                break;

            case LVL_WORLD_ADMIN:
                name_color = '^';
                break;

            case LVL_WORLD_OWNER:
                name_color = '2';
                break;

            case LVL_VIP:
                name_color = '1';
                break;

            case LVL_VIP_PLUS:
                name_color = '3';
                break;

            case LVL_ROYAL:
                name_color = '4';
                break;

            case LVL_PERSONAL:
                name_color = '5';
                break;

            case LVL_MODERATOR:
                name_color = '#';
                m_chat_color = '^';
                break;

            case LVL_MANAGER:
                name_color = 'r';
                m_chat_color = '5';
                break;

            case LVL_DEVELOPER:
            case LVL_OWNER:
                name_color = '6';
                m_chat_color = '5';
                break;
        }

        m_display_name = fmt::format("`{}{}{}``", name_color, m_admin_level >= LVL_MODERATOR ? "@" : "", m_raw_name);
    }

    std::string player::get_display_name()
    {
        std::string name_color = "`w";

        switch (m_admin_level)
        {
            case LVL_BETA_TESTER:
                name_color = '9';
                break;

            case LVL_VIP:
                name_color = '1';
                break;

            case LVL_VIP_PLUS:
                name_color = '3';
                break;

            case LVL_ROYAL:
                name_color = '4';
                break;

            case LVL_PERSONAL:
                name_color = '5';
                break;

            case LVL_MODERATOR:
                name_color = '#';
                m_chat_color = '^';
                break;

            case LVL_MANAGER:
                name_color = 'r';
                break;

            case LVL_DEVELOPER:
            case LVL_OWNER:
                name_color = '6';
                break;
        }

        if (m_admin_level < LVL_VIP)
        {
            if (m_world_manager)
            {
                world* world_;
                if (!m_world_manager->get_world(m_current_world, &world_))
                {
                    send_log("`4Invalid world.``");
                }

                if (world_->is_owner(this))
                    name_color = "`2";
                else if (world_->is_admin(this))
                    name_color = "`^";
            }
        }

        return fmt::format("{}{}``", name_color, m_tank_id);
    }


    void player::update_inventory_item(uint16_t item, uint8_t count, bool adding)
    {
        gameupdatepacket p;

        p.m_type = PACKET_MODIFY_ITEM_INVENTORY;
        p.m_item_id = item;

        if (adding)
            p.m_gained_item_count = count;
        else
            p.m_lost_item_count = count;

        send_packet(4, &p, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
    }
}