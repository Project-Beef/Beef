#include "world.hh"

namespace beef
{
    world::world()
        : m_width(100), m_height(60), m_net_id(0), m_owner_id(-1)
    {
    }

    world::world(const std::string& name)
        : m_name(name),
        m_width(100), m_height(60), m_net_id(0), m_owner_id(-1)
    {
        // std::string file = fmt::format("data/worlds/_{}.bin", name);

        /*if (std::filesystem::exists(file))
        {

        }
        else
        {

        }*/
    }


    void world::add_tile(const tile& tile)
    {
        m_tiles.push_back(tile);
    }

    tile* world::get_tile(int x, int y)
    {
        if (x > m_width || y > m_height) return nullptr;
        if (x < 0 || y < 0) return nullptr;

        return &m_tiles[x + y * m_width];
    }

    tile* world::get_tile(const math::vec2i& pos)
    {
        return get_tile(pos.m_x, pos.m_y);
    }

    bool world::is_locked() const
    {
        return m_owner != -1;
    }

    bool world::is_owner(player* player_) const
    {
        return player_->get_user_id() == m_owner || player_->m_admin_level >= LVL_MODERATOR;
    }

    bool world::is_admin(player* player_) const
    {
        return std::find(m_access.begin(), m_access.end(), player_->get_user_id()) != m_access.end();
    }

    const tile& world::get_c_tile(uint32_t index) const
    {
        return m_tiles[index];
    }

    math::vec2i world::get_tile_pos(uint32_t id) const
    {
        for (size_t i = 0; i < m_tiles.size(); ++i)
        {
            if (m_tiles[i].get_base_uint() == id)
            {
                return { static_cast<int>(i) % m_width, static_cast<int>(i) / m_width };
            }
        }

        return { 0, 0 };
    }

    math::vec2i world::get_tile_pos_by_type(uint32_t type) const
    {
        for (size_t i = 0; i < m_tiles.size(); ++i)
        {
            if (itemdb::get_item(m_tiles[i].get_base_uint()).m_action_type == type)
            {
                return { static_cast<int>(i) % m_width, static_cast<int>(i) / m_width };
            }
        }

        return { 0, 0 };
    }

    size_t world::estimate_memory() const
    {
        size_t consum = 20 + m_name.size() + 16 + (m_objects.size() * 16);

        for (auto& tile : m_tiles)
        {
            tile.estimate_memory_usage(consum);
        }

        // delete all ur worlds and try again
        // its from reading and n

        return consum;
    }

    gameupdatepacket* world::pack()
    {
        size_t alloc = estimate_memory() + sizeof(gameupdatepacket);
        gameupdatepacket* data = static_cast<gameupdatepacket*>(malloc(alloc));
        if (!data)
            return nullptr;

        memset(data, 0, alloc);

        data->m_type = 4;
        data->m_flags = 8;
        data->m_net_id = -1;

        uintmax_t pos = 0;

        uint16_t version = 0xF;
        memcpy(&data->m_data + pos, &version, 2);
        pos += 2;

        uint32_t reserved = 0;
        memcpy(&data->m_data + pos, &reserved, 4);
        pos += 4;

        uint16_t len = static_cast<uint16_t>(m_name.size());
        memcpy(&data->m_data + pos, &len, 2);
        pos += 2;

        memcpy(&data->m_data + pos, m_name.c_str(), len);
        pos += len;

        memcpy(&data->m_data + pos, &m_width, 4);
        pos += 4;

        memcpy(&data->m_data + pos, &m_height, 4);
        pos += 4;

        int tile_count = m_tiles.size();
        memcpy(&data->m_data + pos, &tile_count, 4);
        pos += 4;

        for (auto& tile : m_tiles)
        {
            tile.pack(&data->m_data, pos);
        }

        uint32_t oc = m_objects.size(), drop_id = m_object_id - 1;

        memcpy(&data->m_data + pos, &oc, 4);
        memcpy(&data->m_data + pos + 4, &drop_id, 4);
        pos += 8;
        
        if (oc > 0)
        {
            for (const auto& obj : m_objects)
            {
                memcpy(&data->m_data + pos, &obj.second.m_item_id, 2);
                pos += 2;
                memcpy(&data->m_data + pos, &obj.second.m_pos, sizeof(math::vec2));
                pos += sizeof(math::vec2);
                memcpy(&data->m_data + pos, &obj.second.m_item_amount, 1);
                pos += 1;
                memcpy(&data->m_data + pos, &obj.second.m_flags, 1);
                pos += 1;
                memcpy(&data->m_data + pos, &obj.first, 4);
                pos += 4;
            }
        }

        uint32_t weather = 4, weather1 = 0;
        memcpy(&data->m_data + pos, &weather, 4);
        memcpy(&data->m_data + pos + 4, &weather1, 4);
        pos += 8;

        data->m_data_size = pos;

        io::write_all_bytes("debug_world_packet.bin", data, alloc);

        return data;
    }

    void world::set_new_lock(int user_id)
    {
        m_owner = user_id;
        m_tempo = 100;
        m_lock_flags = 0;
    }

    void world::generate_new_world(int size)
    {
        m_tiles.reserve(size);

        int main_door_x = rand() % 99;

        for (int i = 0; i < size; i++)
        {
            tile t;

            t.set_world(this);
            
            if (i == 3600 + main_door_x)
            {
                t.set_fg(MAINDOOR);
                t.set_label("EXIT");
            }
            else if (i == 3700 + main_door_x)
            {
                t.set_fg(BEDROCK);
            }
            else if (i >= 3800 && i < 5400 && !(rand() % 50))
            {
                t.set_fg(ROCK);
            }
            else if (i >= 3700 && i < 5400)
            {
                if (i > 5000 && !(rand() % 3))
                {
                    t.set_fg(LAVA);
                }
                else
                {
                    t.set_fg(DIRT);
                }
            }
            else if (i >= 5400)
            {
                t.set_fg(BEDROCK);
            }

            if (i >= 3700)
            {
               t.set_bg(CAVEBACKGROUND);
            }

            m_tiles.push_back(std::move(t));
        }
    }

    void world::broadcast(std::function<void(int, player*)> cb)
    {
        for (auto& player : m_players)
        {
            cb(player.first, player.second);
        }
    }

    void world::add_gems_object(int gems, const math::vec2& position)
    {
        world_object obj;
        obj.m_item_id = GEMS;
        obj.m_pos = position;
    
        while (gems != 0)
        {
            if (gems >= 100)
            {
                obj.m_item_amount = 100;
                gems -= 100;
            }
            else if (gems >= 50)
            {
                obj.m_item_amount = 50;
                gems -= 50;
            }
            else if (gems >= 10)
            {
                obj.m_item_amount = 10;
                gems -= 10;
            }
            else if (gems >= 5)
            {
                obj.m_item_amount = 5;
                gems -= 5;
            }
            else if (gems >= 1)
            {
                obj.m_item_amount = 1;
                gems -= 1;
            }
            add_object(obj, true);
        }
    }

    void world::remove_object(uint32_t id)
    {
        if (m_objects.erase(id))
        {
            gameupdatepacket packet{ PACKET_ITEM_CHANGE_OBJECT };
            packet.m_object_change_type = OBJECT_CHANGE_TYPE_REMOVE;
            packet.m_item_net_id = -1;
            packet.m_object_id = id; // k built

            broadcast([&] (int, player* ply) { ply->send_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE); });
        }
    }

    void world::add_object(world_object& object, bool check)
    {
        if (!object.m_item_amount)
            return;

        if (check)
        {
            std::unordered_map<uint32_t, world_object> wrapped_gems;

            auto wrap_gems = [&] (uint8_t amount, uint8_t target, uint8_t target_amount) -> bool
            {
                if (object.m_item_amount == amount)
                {
                    std::unordered_map<uint32_t, world_object> gem_objects;

                    for (auto& pair : wrapped_gems) // what this should beee? wait
                    {
                        if (pair.second.m_item_amount == amount)
                        {
                            gem_objects.insert_or_assign(pair.first, pair.second);
                        }
                    }

                    if (gem_objects.size() == target_amount)
                    {
                        for (const auto& obj : gem_objects)
                        {
                            remove_object(obj.first);
                        }

                        world_object new_object;
                        new_object.m_pos = object.m_pos;
                        new_object.m_item_id = GEMS;
                        new_object.m_item_amount = target_amount;

                        add_object(new_object, true);
                        return true;
                    }
                }
                return false;
            };

            for (auto& obj : m_objects)
            {
                if (obj.second.m_item_id == object.m_item_id &&
                    std::abs(obj.second.m_pos.m_x - object.m_pos.m_x) < 12 &&
				    std::abs(obj.second.m_pos.m_y - object.m_pos.m_y) < 12)
                {
                    if (object.m_item_id == GEMS)
                    {
                        wrapped_gems.insert_or_assign(obj.first, obj.second); // -_- ez
                    }
                    else
                    {
                        if (obj.second.m_item_amount + object.m_item_amount > 200)
                        {
                            object.m_item_amount = static_cast<uint8_t>(obj.second.m_item_amount + object.m_item_amount - 200);
                            obj.second.m_item_amount = 200;
                            modify_object(obj);
                            break;
                        }
                        else
                        {
                            obj.second.m_item_amount += object.m_item_amount;
                            modify_object(obj);
                            return;
                        }
                    }
                }
            }

            if (object.m_item_id == GEMS)
            {
                if (wrap_gems(1, 4, 5))
					return;

				if (wrap_gems(5, 1, 10))
					return;

				if (wrap_gems(10, 4, 50))
					return;

				if (wrap_gems(50, 1, 100))
					return;
            }
        }

        float x = (rand() % 2) == 0 ?
            object.m_pos.m_x + (rand() % 7) :
            object.m_pos.m_x - (rand() % 7);

        float y = (rand() % 2) == 0 ?
            object.m_pos.m_y + (rand() % 7) :
            object.m_pos.m_y - (rand() % 7);

        if (x < 0)
            x = 0;

        if (y < 0)
            y = 0;

        if (x >= m_width * 32)
            x = object.m_pos.m_x;

        if (y >= m_height * 32)
            y = object.m_pos.m_y;
        
        object.m_pos = { x, y };
        uint32_t id = m_object_id++;

        m_objects.insert_or_assign(id, object);

        gameupdatepacket packet{ PACKET_ITEM_CHANGE_OBJECT };
        packet.m_object_change_type = OBJECT_CHANGE_TYPE_ADD;
        packet.m_pos_x = object.m_pos.m_x;
        packet.m_pos_y = object.m_pos.m_y;
        packet.m_item_id = object.m_item_id;
        packet.m_obj_alt_count = static_cast<float>(object.m_item_amount);

        broadcast([&](int, player* ply) { ply->send_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE); });
    }

    void world::modify_object(const std::pair<uint32_t, world_object>& object)
    {
        gameupdatepacket packet { PACKET_ITEM_CHANGE_OBJECT };
        packet.m_object_change_type = OBJECT_CHANGE_TYPE_MODIFY;
        packet.m_pos_x = object.second.m_pos.m_x;
        packet.m_pos_y = object.second.m_pos.m_y;
        packet.m_item_id = object.second.m_item_id;
        packet.m_item_net_id = object.first;
        packet.m_obj_alt_count = static_cast<float>(object.second.m_item_amount); 

        broadcast([&](int, player* ply) { ply->send_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE); });

        m_objects[object.first] = object.second;
    }

    void world::collect_object(player* player_, uint32_t id, const math::vec2& position)
    {
        auto abs_x = std::abs(player_->m_pos.m_x - position.m_x);
	    auto abs_y = std::abs(player_->m_pos.m_y - position.m_y);

        if (abs_x > 52 || abs_y > 52)
        {
            player_->send_variant(variantlist { "OnTalkBubble", player_->get_net_id(), "`w(Too far away)``", 0, 0 });
            return;
        }
                
        if (auto it = m_objects.find(id); it != m_objects.end())
        {
            world_object& obj = it->second;

            float x = obj.m_pos.m_x / 32;
            float y = obj.m_pos.m_y / 32;

            if (x - static_cast<int>(x) >= 0.75f)
            {
                x = static_cast<float>(std::round(x));
            }

            if (y - static_cast<int>(y) >= 0.75f)
            {
                y = static_cast<float>(std::round(y));
            }

            tile* tile_ = get_tile(static_cast<int>(x), static_cast<int>(y));

            if (tile_)
            {
                const item& fg = itemdb::get_item(tile_->get_fg_uint());

                if (fg.m_collision_type == ITEMCOLLISION_NORMAL)
                    return;

                if (obj.m_item_id == GEMS)
                {

                }
                else
                {
                    const item& item_ = itemdb::get_item(obj.m_item_id);

                    bool overload = false;

                    if (player_->has_inventory_item(obj.m_item_id))
                    {
                        uint8_t item_amount = player_->get_inventory_item_count(obj.m_item_id);

                        if (item_amount + obj.m_item_amount > 200)
                        {
                            overload = true;

                            uint8_t new_amount = static_cast<uint8_t>(200 - item_amount);
                            uint8_t new_object_amount = static_cast<uint8_t>(obj.m_item_amount - new_amount);

                            world_object new_object;
                            new_object.m_pos = obj.m_pos;
                            new_object.m_item_id = obj.m_item_id;
                            new_object.m_item_amount = new_object_amount;

                            add_object(new_object, false);

                            player_->add_inventory_item(obj.m_item_id, new_amount);

                            std::string msg = fmt::format("Collected `w{} {}``.", new_amount, item_.m_name);

                            if (item_.m_rarity != 999)
                            {
                                msg.append(fmt::format(" Rarity: `w{}``", item_.m_rarity));
                            }

                            player_->send_variant(variantlist{ "OnConsoleMessage", msg });
                        }
                    }

                    if (!overload)
                    {
                        player_->add_inventory_item(obj.m_item_id, obj.m_item_amount);

                        std::string msg = fmt::format("Collected `w{} {}``.", obj.m_item_amount, item_.m_name);

                        if (item_.m_rarity != 999)
                        {
                            msg.append(fmt::format(" Rarity: `w{}``", item_.m_rarity));
                        }

                        player_->send_variant(variantlist{ "OnConsoleMessage", msg });
                    }
                }

                m_objects.erase(it);

                gameupdatepacket packet{ PACKET_ITEM_CHANGE_OBJECT };
                packet.m_net_id = player_->get_net_id();
                packet.m_object_id = id;

                broadcast([&](int, player* ply) { ply->send_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE); });
            }
        }
    }

    void world::add_player(player* player_)
    {
        player_->set_net_id(++m_net_id);
        player_->m_current_world = m_name;
        m_players.insert_or_assign(m_net_id, player_);
    }

    void world::respawn_player(player* player_, bool killed, int delay)
    {
        if (!killed)
        {
            player_->send_variant(variantlist{ "OnKilled", 1 }, player_->get_net_id());
            player_->send_variant(variantlist{ "OnSetFreezeState", 1 }, player_->get_net_id(), delay);
        }

        delay = delay == -1 ? 2000 : delay;

        math::vec2i pos_i = get_tile_pos_by_type(ITEMTYPE_MAIN_DOOR);
        math::vec2 pos = { static_cast<float>(pos_i.m_x * 32), static_cast<float>(pos_i.m_y * 32) };

        player_->send_variant(variantlist{ "OnSetPos", pos }, player_->get_net_id(), delay);
        player_->send_variant(variantlist{ "OnSetFreezeState", 0 }, player_->get_net_id(), delay);

        gameupdatepacket* pck = variantlist{ "OnPlayPositioned", "audio/teleport.wav" }.pack();
        pck->m_delay = delay;
        pck->m_net_id = player_->get_net_id();

        broadcast([&](int, player* p) { p->send_packet(NET_MESSAGE_GAME_PACKET, pck, pck->m_data_size + sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE); });
    }

    void world::remove_player(int net_id)
    {
        auto it = std::find_if(m_players.begin(), m_players.end(),
                        [&net_id](const auto & p) { return p.second->get_net_id() == net_id; });

        if (it != m_players.end())
        {
            it->second->m_current_world = "EXIT";
            m_players.erase(it);
        }
    }
}
