#include "tile.hh"
#include "world.hh"
#include "itemdb/item_flags.hh"

namespace beef
{
    tile::tile()
        : m_fg(0), m_bg(0), m_lp(0), m_fl(0)
    {
    }

    tile::tile(uint16_t fg, uint16_t bg, uint16_t lp, uint16_t ext)
        : m_fg(fg), m_bg(bg), m_lp(lp), m_fl(ext)
    {
    }

    void tile::set_fg(uint16_t id)
    {
        m_fg = id;
    }

    void tile::set_bg(uint16_t id)
    {
        m_bg = id;
    }

    void tile::set_world(world* world_)
    {
        m_world = world_;
    }


    const item& tile::get_base_item() const
    {
        return itemdb::get_item(m_fg != 0 ? m_fg : m_bg);
    }

    uint16_t tile::get_base_uint() const
    {
        return m_fg != 0 ? m_fg : m_bg;
    }

    void tile::remove_base()
    {
        uint16_t& id = m_fg != 0 ? m_fg : m_bg;
        reset_hit();

        const item& it = itemdb::get_item(id);
        m_fl = 0;

        if (m_locked)
            set_flag(TILEFLAG_LOCKED);

        id = 0;
    }

    void tile::remove_lock()
    {
        m_lp = 0;
        m_lock_flags = 0;
        set_locked(false);
        remove_flag(TILEFLAG_LOCKED);
    }

    uint16_t tile::get_fg_uint() const
    {
        return m_fg;
    }

    uint16_t tile::get_bg_uint() const
    {
        return m_bg;
    }

    uint8_t tile::indicate_hit()
    {
        m_last_hitten = clock::now();
        return ++m_hit_count;
    }

    void tile::set_locked(bool locked)
    {
        m_locked = locked;
    }

    bool tile::is_locked() const
    {
        return m_locked;
    }

    void tile::reset_hit()
    {
        m_hit_count = 0;
        m_last_hitten = {};
    }

        
    time_point tile::get_last_hit_time() const
    {
        return m_last_hitten;
    }

    int tile::get_owner_id() const
    {
        return m_owner;
    }

    uint8_t tile::get_hit_count() const
    {
        return m_hit_count;
    }

    uint8_t tile::get_display_item() const
    {
        return m_display_item;
    }

    void tile::set_display_item(uint16_t id)
    {
        set_flag(TILEFLAG_TILEEXTRA);
            
        m_display_item = id;
    }

    void tile::reset_flag()
    {
        m_fl = 0;
    }

    void tile::set_flag(uint16_t flags)
    {
        m_fl |= flags;
    }

    void tile::remove_flag(uint16_t flags)
    {
        m_fl &= ~flags;
    }


    void tile::set_new_seed()
    {
        set_flag(TILEFLAG_SEED);
        set_flag(TILEFLAG_TILEEXTRA);

        m_fruit_count = (rand() % 6) + 1;
        m_last_planted = clock::now();
    }

    void tile::set_new_lock(int user_id)
    {
        m_owner = user_id;
        m_lock_flags = 0;

        if (user_id != -1)
        {
            set_flag(TILEFLAG_TILEEXTRA);
        }
    }

    void tile::set_new_lock(int user_id, const math::vec2i& parent)
    {
        m_lp = parent.m_x + parent.m_y * m_world->m_width;
        m_owner = user_id;
        m_lock_flags = 0;
        set_flag(TILEFLAG_LOCKED);
        set_locked(true);
    }

    void tile::set_label(const std::string& label)
    {
        // come to remove_base
        set_flag(TILEFLAG_TILEEXTRA);
        m_label = label;
    }

    void tile::remove_label()
    {
        remove_flag(TILEFLAG_TILEEXTRA);
        m_label = "";
    }

    const std::string& tile::get_label() const
    {
        return m_label;
    }


    void tile::pack(uint8_t* data, uintmax_t& pos)
    {
        memcpy(data + pos, &m_fg, 2);
        memcpy(data + pos + 2, &m_bg, 2);
        memcpy(data + pos + 4, &m_lp, 2);
        memcpy(data + pos + 6, &m_fl, 2);

        pos += 8;

        if (m_locked)
        {
            memcpy(data + pos, &m_lp, 2);
            pos += 2;
        }

        if (m_fl & TILEFLAG_TILEEXTRA)
            handle_tile_extra(data, pos, this, false);
    }

    size_t tile::estimate_memory_usage(size_t& consum) const
    {
        consum += 8;

        if (m_locked)
            consum += 2;

        if (m_fl & TILEFLAG_TILEEXTRA)
        {
            const item& it = itemdb::get_item(m_fg);

            switch (it.m_action_type)
            {
                case ITEMTYPE_DOOR:
                case ITEMTYPE_MAIN_DOOR:
                {
                    consum += 4 + m_label.size();
                    break;
                }

                case ITEMTYPE_SIGN:
                {
                    consum += 7 + m_label.size();
                    break;
                }

                case ITEMTYPE_SEED:
                {
                    consum += 14;
                    break;
                }

                case ITEMTYPE_DISPLAY_BLOCK:
                {
                    consum += 5;
                    break;
                }
                
                case ITEMTYPE_LOCK:
                {
                    if (item_is_world_lock(it))
                    {
                        consum += 22 + (m_world->m_access.size() * 4);
                    }
                    else
                    {
                        consum += 18 + (m_access.size() * 4);
                    }
                    break;
                }

                default: 
                    break;
            }
        }

        return consum;
    }


    uint16_t tile::get_lp() const
    {
        return m_lp;
    }

    uint16_t tile::get_fl() const
    {
        return m_fl;
    }


    void handle_tile_extra(uint8_t* data, uintmax_t& pos, const tile* tile, bool to_database)
    {
        const item& it = itemdb::get_item(tile->m_fg);

        switch (it.m_action_type)
        {
            case ITEMTYPE_DOOR:
            case ITEMTYPE_MAIN_DOOR:
            {
                data[pos++] = 1;
                uint16_t str_len = static_cast<uint16_t>(tile->m_label.size());
                memcpy(data + pos, &str_len, 2);
                pos += 2;

                memcpy(data + pos, tile->m_label.c_str(), str_len);
                pos += str_len;

                data[pos++] = 0;
                break;
            }

            case ITEMTYPE_SIGN:
            {
                data[pos++] = 2;

                uint16_t len = static_cast<uint16_t>(tile->m_label.size());
                memcpy(data + pos, &len, 2);
                pos += 2;

                memcpy(data + pos, tile->m_label.c_str(), len);
                pos += len;

                int end_marker = -1;
                memcpy(data + pos, &end_marker, 4);
                pos += 4;
                break;
            }

            case ITEMTYPE_SEED:
            {
                data[pos++] = 4;

                if (to_database)
                {
                    uint64_t time = tile->m_last_planted.time_since_epoch().count();

                    memcpy(data + pos, &time, 8);
                    pos += 8;
                }
                else
                {
                    uint32_t time = std::chrono::duration_cast<std::chrono::seconds>(clock::now() - tile->m_last_planted).count();

                    memcpy(data + pos, &time, 4);
                    pos += 4;
                }

                memcpy(data + pos, &tile->m_fruit_count, 1);
                pos += 1;

                break;
            }

            case ITEMTYPE_DISPLAY_BLOCK:
            {
                data[pos++] = 23;

                memcpy(data + pos, &tile->m_display_item, 2);
                pos += 4;
                
                break;
            }

            case ITEMTYPE_LOCK:
            {
                data[pos++] = 3;

                if (item_is_world_lock(it))
                {
                    int tempo = -tile->m_world->m_tempo;
                    int access_size = static_cast<int>(tile->m_world->m_access.size()) + 1;

                    memcpy(data + pos, &tile->m_world->m_lock_flags, 1);
                    pos += 1;
                    memcpy(data + pos, &tile->m_world->m_owner, 4);
                    pos += 4;
                    memcpy(data + pos, &access_size, 4);
                    pos += 4;

                    for (const auto& id : tile->m_world->m_access)
                    {
                        memcpy(data + pos, &id, 4);
                        pos += 4;
                    }

                    memcpy(data + pos, &tempo, 4);
                    pos += 4;
                }
                else
                {
                    int access_size = static_cast<int>(tile->m_access.size());

                    memcpy(data + pos, &tile->m_lock_flags, 1);
                    pos += 1;
                    memcpy(data + pos, &tile->m_owner, 4);
                    pos += 4;
                    memcpy(data + pos, &access_size, 4);
                    pos += 4;

                    for (const auto& id : tile->m_world->m_access)
                    {
                        memcpy(data + pos, &id, 4);
                        pos += 4;
                    }
                }

                int reserved = 1, reserved2 = 0;
                memcpy(data + pos, &reserved, 4);
                memcpy(data + pos + 4, &reserved2, 4);
                pos += 8;

                break;
            }

            default:
            {
                fmt::print("unsupported item which needs extra data is placed!! {} ({})\n", it.m_name, it.m_action_type);
                break;
            }
        }
    }

    uint8_t tile::get_fruit_count() const
    {
        return m_fruit_count;
    }

    time_point tile::get_last_planted() const
    {
        return m_last_planted;
    }

    void read_tile_extra(uint8_t* data, uintmax_t& pos, tile* tile)
    {
        const item& it = itemdb::get_item(tile->m_fg);

        switch (it.m_action_type)
        {
            case ITEMTYPE_DOOR:
            case ITEMTYPE_MAIN_DOOR:
            {
                uint16_t len = static_cast<uint16_t>(tile->m_label.size());
                memcpy(&len, data + ++pos, 2);
                pos += 2;

                tile->m_label = std::string(reinterpret_cast<char*>(data + pos++), len);
                pos += len;

                break;
            }

            case ITEMTYPE_SIGN:
            {
                uint16_t len = static_cast<uint16_t>(tile->m_label.size());
                memcpy(&len, data + ++pos, 2);
                pos += 2;

                tile->m_label = std::string(reinterpret_cast<char*>(data + pos), len);
                pos += len;
                pos += 4;

                break;
            }

            case ITEMTYPE_SEED:
            {
                uint64_t time = 0;

                memcpy(&time, data + ++pos, 8);
                pos += 8;
                memcpy(&tile->m_fruit_count, data + pos, 1);
                pos += 1;

                tile->m_last_planted = time_point(std::chrono::nanoseconds(time));

                break;
            }

            case ITEMTYPE_DISPLAY_BLOCK:
            {
                memcpy(&tile->m_display_item, data + ++pos, 2);
                pos += 4;
                
                break;
            }

            case ITEMTYPE_LOCK:
            {
                pos += 1;

                if (item_is_world_lock(it))
                {
                    int access_size;

                    memcpy(&tile->m_world->m_lock_flags, data + pos, 1);
                    pos += 1;
                    memcpy(&tile->m_world->m_owner, data + pos, 4);
                    pos += 4;
                    memcpy(&access_size, data + pos, 4);
                    pos += 4;

                    for (int i = 0; i < access_size - 1; ++i)
                    {
                        int id;
                        memcpy(&id, data + pos, 4);
                        pos += 4;
                        tile->m_world->m_access.emplace_back(id);
                    }

                    memcpy(&tile->m_world->m_tempo, data + pos, 4);
                    pos += 4;

                    tile->m_owner = tile->m_world->m_owner;
                    tile->m_access = tile->m_world->m_access;
                    tile->m_lock_flags = tile->m_world->m_lock_flags;
                }
                else
                {
                    int access_size;

                    memcpy(&tile->m_lock_flags, data + pos, 1);
                    pos += 1;
                    memcpy(&tile->m_owner, data + pos, 4);
                    pos += 4;
                    memcpy(&access_size, data + pos, 4);
                    pos += 4;

                    for (int i = 0; i < access_size; ++i)
                    {
                        int id;
                        memcpy(&id, data + pos, 4);
                        pos += 4;
                        tile->m_access.emplace_back(id);
                    }
                }

                pos += 8;

                break;
            }

            default:
            {
                // fmt::print("unsupported item which needs extra data is placed!! type: {} ({})\n", it.m_name, it.m_action_type);
                break;
            }
        }
    }
}
