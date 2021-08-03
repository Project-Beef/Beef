#include "database.hh"

#include "constants/config.hh"
#include "itemdb/item_flags.hh"
#include "utils/io.hh"
#include "world/tile.hh"
#include "world/world.hh"
#include "player/player.hh"
#include <time.h>

namespace beef::database
{
    bool read_user_id(server* server) noexcept
    {
        uintmax_t filesize;
        int* data = static_cast<int*>(io::read_all_bytes(config::settings::fs::server_data_file, filesize));
        if (!data)
            return false;

        server->set_user_id(*data);

        free(data);
        return true;
    }

    void write_user_id(server* server) noexcept
    {
        int uid = server->get_user_id();
        io::write_all_bytes(config::settings::fs::server_data_file, &uid, 4);
    }

    bool read_player(player* player_, const std::string_view& file) noexcept
    {
        uintmax_t filesize;
        uint8_t* data = static_cast<uint8_t*>(io::read_all_bytes(file, filesize));

        if (!data)
            return false;

        uintmax_t pos = 0;

        auto read_str = [&pos, &data] (std::string& str) {
            uint8_t len = data[pos++];
            str = std::string(reinterpret_cast<char*>(data + pos), len);
            pos += len;
        };

        auto read_int = [&pos, &data] (void* d, size_t size = 4) {
            memcpy(d, data + pos, size);
            pos += size;
        };

        uint32_t version = 0;
        read_int(&version);

        int user_id = 0;
        read_int(&user_id);
        player_->set_user_id(user_id);

        read_int(&player_->m_admin_level);
        read_int(&player_->m_created_at, sizeof(time_t));

        read_str(player_->m_raw_name);
        read_str(player_->m_raw_pass);

        read_str(player_->m_email);
        read_str(player_->m_discord);
        read_str(player_->m_raw_ip);

        read_int(&player_->m_gems);
        read_int(&player_->m_skin_color);

        read_int(&player_->m_cloth.m_hair);
        read_int(&player_->m_cloth.m_shirt);
        read_int(&player_->m_cloth.m_pants);
        read_int(&player_->m_cloth.m_feet);
        read_int(&player_->m_cloth.m_face);
        read_int(&player_->m_cloth.m_hand);
        read_int(&player_->m_cloth.m_back);
        read_int(&player_->m_cloth.m_mask);
        read_int(&player_->m_cloth.m_necklace);
        read_int(&player_->m_cloth.m_ances);

        read_int(&player_->m_inv.m_size);

        size_t item_count = 0;
        read_int(&item_count);

        for (size_t i = 0; i < item_count; ++i)
        {
            uint16_t item_id = 0;
            uint8_t item_count = 0;

            read_int(&item_id, 2);
            read_int(&item_count, 1);


            player_->m_inv.m_items[item_id] = item_count;
        }

        if (version >= 2)
        {
            read_int(&player_->m_banned_at, 8);
            read_int(&player_->m_banned_time, 8);
        }

        free(data);
        return true;
    }

    bool write_player(const player* player_, const std::string_view& file) noexcept
    {
        size_t size = sizeof(player) + (3 * player_->m_inv.m_items.size());
        uint8_t* data = static_cast<uint8_t*>(malloc(size));

        if (!data)
            return false;

        memset(data, 0, size);

        uintmax_t pos = 0;

        auto write_int = [&pos, &data] (const void* d, size_t size = 4) {
            memcpy(data + pos, d, size);
            pos += size;
        };

        auto write_str = [&pos, &data] (const std::string& str) {
            uint8_t len = static_cast<uint8_t>(str.size());
            data[pos++] = len;

            memcpy(data + pos, str.data(), len);
            pos += len;
        };

        write_int(&config::db::version);

        int user_id = player_->get_user_id();
        write_int(&user_id);
        write_int(&player_->m_admin_level);
        write_int(&player_->m_created_at, sizeof(time_t));

        write_str(player_->m_raw_name);
        write_str(player_->m_raw_pass);
        write_str(player_->m_email);
        write_str(player_->m_discord);
        write_str(player_->m_raw_ip);

        write_int(&player_->m_gems);
        write_int(&player_->m_skin_color);

        write_int(&player_->m_cloth.m_hair);
        write_int(&player_->m_cloth.m_shirt);
        write_int(&player_->m_cloth.m_pants);
        write_int(&player_->m_cloth.m_feet);
        write_int(&player_->m_cloth.m_face);
        write_int(&player_->m_cloth.m_hand);
        write_int(&player_->m_cloth.m_back);
        write_int(&player_->m_cloth.m_mask);
        write_int(&player_->m_cloth.m_necklace);
        write_int(&player_->m_cloth.m_ances);

        write_int(&player_->m_inv.m_size);

        size_t item_count = player_->m_inv.m_items.size();
        write_int(&item_count);

        for (const auto& item : player_->m_inv.m_items)
        {
            write_int(&item.first, 2);
            write_int(&item.second, 1);
        }

        write_int(&player_->m_banned_at, 8);
        write_int(&player_->m_banned_time, 8);

        io::write_all_bytes(file, data, pos);
        free(data);

        return true;
    }

    bool read_world(world* world_, const std::string_view& file) noexcept
    {
        uintmax_t filesize;
        uint8_t* data = static_cast<uint8_t*>(io::read_all_bytes(file, filesize));
        if (!data)
            return false;

        uintmax_t pos = 0;

        auto read_str = [&pos, &data] (std::string& str) {
            uint8_t len = data[pos++];
            str = std::string(reinterpret_cast<char*>(data + pos), len);
            pos += len;
        };

        auto read_int = [&pos, &data] (void* d, size_t size = 4) {
            memcpy(d, data + pos, size);
            pos += size;
        };

        uint32_t version = 0;
        read_int(&version);

        read_str(world_->m_name);
        read_int(&world_->m_width);
        read_int(&world_->m_height);

        size_t tiles = 6000;
        read_int(&tiles);

        world_->m_tiles.reserve(tiles);
        for (size_t i = 0; i < tiles; ++i)
        {
            bool locked;
            uint16_t fg, bg, lp, fl;

            read_int(&fg, 2);
            read_int(&bg, 2);
            read_int(&lp, 2);
            read_int(&fl, 2);
            read_int(&locked, 1);

            tile& t = world_->m_tiles.emplace_back(tile { fg, bg, lp, fl });
            t.set_world(world_);
            t.set_locked(locked);

            if (fl & TILEFLAG_TILEEXTRA)
                read_tile_extra(data, pos, &t);
        }

        if (version >= 1)
        {
            uint32_t oc;
            read_int(&oc);
            read_int(&world_->m_object_id);

            if (oc > 0)
            {
                for (uint32_t i = 0; i < oc; ++i)
                {
                    uint32_t id;
                    world_object obj;

                    memcpy(&obj.m_item_id, data + pos, 2);
                    pos += 2;
                    memcpy(&obj.m_pos, data + pos, 8);
                    pos += 8;
                    memcpy(&obj.m_item_amount, data + pos, 1);
                    pos += 1;
                    memcpy(&obj.m_flags, data + pos, 1);
                    pos += 1;
                    memcpy(&id, data + pos, 4);
                    pos += 4;
                    
                    world_->m_objects.insert_or_assign(id, obj);
                }
            }
        }

        free(data);
        return true;
    }

    bool write_world(const world* world_, const std::string_view& file) noexcept
    {
        size_t alloc = world_->estimate_memory() + 12 + world_->m_width * world_->m_height + (16 * world_->m_objects.size());
        uint8_t* data = static_cast<uint8_t*>(malloc(alloc));

        uintmax_t pos = 0;

        if (!data)
            return false;

        auto write_int = [&pos, &data] (const void* d, size_t size = 4) {
            memcpy(data + pos, d, size);
            pos += size;
        };

        auto write_str = [&pos, &data] (const std::string& str) {
            uint8_t len = static_cast<uint8_t>(str.size());
            data[pos++] = len;

            memcpy(data + pos, str.data(), len);
            pos += len;
        };

        write_int(&config::db::version);
        write_str(world_->m_name);

        write_int(&world_->m_width);
        write_int(&world_->m_height);

        uint32_t tc = world_->m_tiles.size();
        write_int(&tc);

        for (const auto& tile : world_->m_tiles)
        {
            bool locked = tile.is_locked();
            uint16_t fg = tile.get_fg_uint(), bg = tile.get_bg_uint(), lp = tile.get_lp(), fl = tile.get_fl();

            write_int(&fg, 2);
            write_int(&bg, 2);
            write_int(&lp, 2);
            write_int(&fl, 2);
            write_int(&locked, 1);

            if (fl & TILEFLAG_TILEEXTRA)
                handle_tile_extra(data, pos, &tile, true);
        }

        uint32_t oc = world_->m_objects.size();

        write_int(&oc, 4);
        write_int(&world_->m_object_id, 4);
        
        if (oc > 0)
        {
            for (const auto& obj : world_->m_objects)
            {
                write_int(&obj.second.m_item_id, 2);
                write_int(&obj.second.m_pos, 8);
                write_int(&obj.second.m_item_amount, 1);
                write_int(&obj.second.m_flags, 1);
                write_int(&obj.first);
            }
        }

        io::write_all_bytes(file, data, pos);
        free(data);

        return true;
    }
}