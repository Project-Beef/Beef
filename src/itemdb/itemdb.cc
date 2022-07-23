#include "itemdb.hh"

#include "constants/config.hh"

#include "itemdb/item_types.hh"

#include "utils/io.hh"
#include "utils/text.hh"

#include "packet/gameupdatepacket.hh"

#include <fstream>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <filesystem>

#include <stdint.h>

#include <fmt/core.h>

namespace beef
{
    void itemdb::interface__kill()
    {
        free(m_data);
        free(m_p_data);
        m_items.clear();
    }

    void itemdb::interface__init(const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
        {
            uint64_t size = std::filesystem::file_size(path);
            m_data = static_cast<uint8_t*>(malloc(size));

            std::ifstream file(path, std::ios::binary);
            if (!file.bad())
            {
                file.read(reinterpret_cast<char*>(m_data), size);

                m_hash = text::proton::hash(m_data, size);
                fmt::print("- items.dat hash: {}\n", m_hash);

                file.close();
            }
            else
            {
                throw std::runtime_error("failed to open file.");
            }

            uint32_t pos = 0;

            memcpy(&m_version, m_data, 2);
            memcpy(&m_item_count, m_data + 2, 4);
            pos += 6;

            fmt::print("items.dat version: {}, item count: {}\n", m_version, m_item_count);
            if (m_version < 11)
                fmt::print("items.dat version possibly unsupported\n");

            for (uint32_t i = 0; i < m_item_count; ++i)
            {
                item item;

                memcpy(&item.m_id, m_data + pos, 4);
                pos += 4;

                item.m_editable_type = m_data[pos];
                pos += 1;

                item.m_item_category = m_data[pos];
                pos += 1;

                item.m_action_type = m_data[pos];
                pos += 1;

                item.m_hit_sound_type = m_data[pos];
                pos += 1;
                
                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    
                    item.m_name = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    item.m_name = text::proton::cypher(item.m_name, item.m_id);

                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_texture = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                memcpy(&item.m_texture_hash, m_data + pos, 4);
                pos += 4;

                item.m_item_kind = m_data[pos];
                pos += 1;

                memcpy(&item.m_flags1, m_data + pos, 4);
                pos += 4;

                item.m_texture_x = m_data[pos];
                pos += 1;

                item.m_texture_y = m_data[pos];
                pos += 1;

                item.m_spread_type = m_data[pos];
                pos += 1;

                item.m_is_stripey_wallpaper = m_data[pos];
                pos += 1;

                item.m_collision_type = m_data[pos];
                pos += 1;

                item.m_break_hits = m_data[pos] / 6; // should be divided by 6?
                pos += 1;

                memcpy(&item.m_reset_time, m_data + pos, 4);
                pos += 4;

                item.m_clothing_type = m_data[pos++];

                memcpy(&item.m_rarity, m_data + pos, 2);
                pos += 2;

                item.m_max_amount = m_data[pos];
                pos += 1;

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_extra_file = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                memcpy(&item.m_extra_file_hash, m_data + pos, 4);
                pos += 4;

                memcpy(&item.m_audio_volume, m_data + pos, 4);
                pos += 4;

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_pet_name = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_pet_prefix = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_pet_suffix = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_pet_ability = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                item.m_seed_base = m_data[pos];
                pos += 1;

                item.m_seed_overlay = m_data[pos];
                pos += 1;

                item.m_tree_base = m_data[pos];
                pos += 1;

                item.m_tree_leaves = m_data[pos];
                pos += 1;

                memcpy(&item.m_seed_color, m_data + pos, 4);
                pos += 4;

                memcpy(&item.m_seed_overlay_color, m_data + pos, 4);
                pos += 4;

                memcpy(&item.m_ingredient, m_data + pos, 4); // deleted ingredients?
                pos += 4;

                memcpy(&item.m_grow_time, m_data + pos, 4);
                pos += 4;

                memcpy(&item.m_flags2, m_data + pos, 2);
                pos += 2;

                memcpy(&item.m_rayman, m_data + pos, 2);
                pos += 2;

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_extra_options = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_texture2 = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                {
                    uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                    item.m_extra_options2 = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                    pos += 2 + str_len;
                }

                // reserved
                memcpy(&item.m_reserved, m_data + pos, 80);
                pos += 80;

                if (m_version >= 11)
                {
                    {
                        uint16_t str_len = *reinterpret_cast<uint16_t*>(&m_data[pos]);
                        item.m_punch_options = std::string(reinterpret_cast<char*>(m_data + pos + 2), str_len);
                        pos += 2 + str_len;
                    }

                    if (m_version >= 12)
                    {
                        memcpy(&item.m_flags3, m_data + pos, 4);
                        pos += 4;

                        // reserved
                        memcpy(&item.m_bodypart, m_data + pos, 9);
                        pos += 9;

                        if (m_version >= 13)
                        {
                            memcpy(&item.m_flags4, m_data + pos, 4);
                            // flags?
                            pos += 4;
				
		            if (m_version >= 14)
			    {
			        memcpy(&item.m_flags5, m_data + pos, 4); // I don't know what is this for, but it's 4 bytes!
				pos += 4;
			    }
                        }
                    }
                }

                if (i != item.m_id)
                    fmt::print("items are unordered: {}/{}\n", i, item.m_id);

                switch (item.m_action_type)
                {
                    case 2:
                    case 3:
                    case 0xA:
                    case 0xD:
                    case 0x13:
                    case 0x1A:
                    case 0x21:
                    case 0x22:
                    case 0x24:
                    case 0x26:
                    case 0x28:
                    case 0x2B:
                    case 0x2E:
                    case 0x2F:
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                    case 0x3B:
                    case 0x3D:
                    case 0x3E:
                    case 0x3F:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                    case 0x44:
                    case 0x47:
                    case 0x48:
                    case 0x49:
                    case 0x4A:
                    case 0x4B:
                    case 0x4C:
                    case 0x4D:
                    case 0x4E:
                    case 0x4F:
                    case 0x50:
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x56:
                    case 0x57:
                    case 0x58:
                    case 0x59:
                    case 0x5B:
                    case 0x5C:
                    case 0x5D:
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 0x63:
                    case 0x64:
                    case 0x65:
                    case 0x66:
                    case 0x67:
                    case 0x68:
                    case 0x69:
                    case 0x6A:
                    case 0x6D:
                    case 0x6F:
                    case 0x71:
                    case 0x73:
                    case 0x74:
                    case 0x75:
                    case 0x76:
                    case 0x7B:
                    case 0x7C:
                    case 0x7D:
                    case 0x7E:
                    case 0x7F:
                    case 0x82:
                    case 0x83:
                    case 0x85:
                    case 0x86:
                    {
                        item.m_has_extra = true;
                        break;
                    }
                }

                if (!item.m_texture.empty())
                {
                    uintmax_t filesize = 0;
                    std::string file = fmt::format("data/cache/game/{}", item.m_texture);

                    void* data = nullptr;

                    if (std::filesystem::exists(file))
                        data = io::read_all_bytes(file, filesize);

                    if (!data) {}
                        // fmt::print("failed to open texture {}\n", item.m_texture);
                    else
                    {
                        item.m_texture_hash = text::proton::hash(data, filesize);
                        free(data); // free u forgot to free(data) :( - kevz
                    }
                }

                if (!item.m_extra_file.empty())
                {
                    uintmax_t filesize = 0;
                    std::string file = fmt::format("data/cache/game/{}", item.m_extra_file);

                    void* data = nullptr;

                    if (std::filesystem::exists(file))
                        data = io::read_all_bytes(file, filesize);

                    if (!data) {}
                        // fmt::print("failed to open {}\n", item.m_texture);
                    else
                    {
                        item.m_extra_file_hash = text::proton::hash(data, filesize);
                        free(data); // free u forgot to free(data) :( - kevz
                    }

                    if (item.m_extra_file.rfind("_icon.rttex") != std::string::npos)
                    {
                        fmt::print("found: {} {} - {}\n", item.m_extra_file, item.m_name, item.m_id);
                    }
                }

                m_items.push_back(std::move(item));
            }

            // some hacky quirks to make dis shit faster!
            uint32_t p_size = 4 + sizeof(gameupdatepacket) + size;
            m_p_data = static_cast<uint8_t*>(malloc(p_size));

            memset(m_p_data, 0, p_size);
            
            uint32_t type = 4;
            memcpy(m_p_data, &type, 4);

            // very very messy shit
            reinterpret_cast<gameupdatepacket*>(m_p_data + 4)->m_type = 16;
            reinterpret_cast<gameupdatepacket*>(m_p_data + 4)->m_flags |= 0x8;
            reinterpret_cast<gameupdatepacket*>(m_p_data + 4)->m_data_size = size;
            memcpy(&reinterpret_cast<gameupdatepacket*>(m_p_data + 4)->m_data, m_data, reinterpret_cast<gameupdatepacket*>(m_p_data + 4)->m_data_size);
        }
        else
            fmt::print("items.dat doesn't exists\n");
    }

    void itemdb::interface__encode(const std::filesystem::path &path)
    {
        uint32_t pos = 0;
        size_t size = m_items.size() * sizeof(item) * 10;
        uint8_t* data = static_cast<uint8_t*>(malloc(size));

        if (!data)
        {
            fmt::print("failed to allocate enough data for encoding items.dat\n");
            return;
        }

        auto write_string = [&](const std::string& str)
        {
            uint16_t strsize = static_cast<uint16_t>(str.size());
            memcpy(data + pos, &strsize, 2);
            pos += 2;
            for (int i = 0; i < strsize; ++i)
                data[pos++] = static_cast<uint8_t>(str[i]);
	    };

        auto write_string_encrypted = [&](int id, const std::string& str) 
        {
            uint16_t strsize = static_cast<uint16_t>(str.size());
            memcpy(data + pos, &strsize, 2);
            pos += 2;

            std::string encrypted = text::proton::cypher(str, id);

            for (int i = 0; i < strsize; ++i)
                data[pos++] = static_cast<uint8_t>(encrypted[i]);
		};

        memset(data, 0, size);
        memcpy(data + pos, &m_version, 2);
        memcpy(data + pos + 2, &m_item_count, 4);
        pos += 6;


        for (const item& item : m_items)
        {
            uint32_t id = item.m_id;

            memcpy(data + pos, &id, 4);
            pos += 4;
            memcpy(data + pos, &item.m_editable_type, 1);
            pos += 1;
            memcpy(data + pos, &item.m_item_category, 1);
            pos += 1;
            memcpy(data + pos, &item.m_action_type, 1);
            pos += 1;
            memcpy(data + pos, &item.m_hit_sound_type, 1);
            pos += 1;
            
            write_string_encrypted(item.m_id, item.m_name);
            write_string(item.m_texture);

            memcpy(data + pos, &item.m_texture_hash, 4);
            pos += 4;
            memcpy(data + pos, &item.m_item_kind, 1); // yes 
            pos += 1;
            memcpy(data + pos, &item.m_flags1, 4);
            pos += 4;
            memcpy(data + pos, &item.m_texture_x, 1);
            pos += 1;
            memcpy(data + pos, &item.m_texture_y, 1);
            pos += 1;
            memcpy(data + pos, &item.m_spread_type, 1);
            pos += 1;
            memcpy(data + pos, &item.m_is_stripey_wallpaper, 1);
            pos += 1;
            memcpy(data + pos, &item.m_collision_type, 1);
            pos += 1;
            
            uint8_t breakHits = item.m_break_hits * 6;
            memcpy(data + pos, &breakHits, 1);
            pos += 1;

            memcpy(data + pos, &item.m_reset_time, 4);
            pos += 4;
            memcpy(data + pos, &item.m_clothing_type, 1); // actually, can't u just do item.m_bodypart here?
            pos += 1;
            memcpy(data + pos, &item.m_rarity, 2);
            pos += 2;
            memcpy(data + pos, &item.m_max_amount, 1);
            pos += 1;

            write_string(item.m_extra_file);

            memcpy(data + pos, &item.m_extra_file_hash, 4);
            pos += 4;
            memcpy(data + pos, &item.m_audio_volume, 4); 
            pos += 4;

            write_string(item.m_pet_name);
            write_string(item.m_pet_prefix);
            write_string(item.m_pet_suffix);
            write_string(item.m_pet_ability);

            memcpy(data + pos, &item.m_seed_base, 1);
            pos += 1;
            memcpy(data + pos, &item.m_seed_overlay, 1);
            pos += 1;
            memcpy(data + pos, &item.m_tree_base, 1);
            pos += 1;
            memcpy(data + pos, &item.m_tree_leaves, 1);
            pos += 1;
            memcpy(data + pos, &item.m_seed_color, 4);
            pos += 4;
            memcpy(data + pos, &item.m_seed_overlay_color, 4);
            pos += 4;
            memcpy(data + pos, &item.m_ingredient, 4);
            pos += 4;
            memcpy(data + pos, &item.m_grow_time, 4);
            pos += 4;
            memcpy(data + pos, &item.m_flags2, 2);
            pos += 2;
            memcpy(data + pos, &item.m_rayman, 2);
            pos += 2;

            write_string(item.m_extra_options);
            write_string(item.m_texture2);
            write_string(item.m_extra_options2);

            memcpy(data + pos, item.m_reserved, 80);
            pos += 80;

            if (m_version >= 11)
            {
                write_string(item.m_punch_options);
            }

            if (m_version >= 12)
            {
                memcpy(data + pos, &item.m_flags3, 4);
                pos += 4;
                memcpy(data + pos, item.m_bodypart, 9);
                pos += 9;
            }

            if (m_version >= 13)
            {
                memcpy(data + pos, &item.m_flags4, 4);
                pos += 4;
            }
            
	    if (m_version >= 14)
	    {
		 memcpy(data + pos, &item.m_flags5, 4); // Add it for encoding tho :)
		 pos += 4;
	    }
        }



        std::ofstream fs(path, std::ios::binary);
        fs.write(reinterpret_cast<char*>(data), pos);
        fs.close();
        
        free(data);
	}

    void itemdb::interface__rehash()
    {
        for (auto& item : m_items)
        {
            //if (item.)
        }
    }

    itemdb::~itemdb()
    {
        free(m_data);
        free(m_p_data);
    }

    const item& itemdb::interface__get_item(uint32_t id) const
    {
        if (id > m_items.size())
            return m_items.at(0);

        return m_items.at(id);
    }

    item& itemdb::interface__get_item_unsafe(uint32_t id)
    {
        return m_items.at(id);
    }

    const item& itemdb::interface__get_item(const std::string& name) const
    {
        for (auto& item : m_items)
        {
            if (item.m_name == name)
            {
                return item;
            }
        }

        return m_items.at(0);
    }

    std::vector<item> itemdb::interface__get_items_matching(const std::string& name) const
    {
        std::vector<item> items;

        std::string lower_name;
        std::for_each(name.begin(), name.end(), [&] (auto& ch) { lower_name += std::tolower((uint8_t)ch); });

        for (auto& item : m_items)
        {
            std::string lower_case;
            std::for_each(item.m_name.begin(), item.m_name.end(), [&] (auto& ch) { lower_case += std::tolower((uint8_t)ch); });
            
            if (lower_case.find(lower_name) == 0)
            {

                items.push_back(item);
            }
        }

        return items;
    }

    const std::vector<item>& itemdb::interface__get_items()
    {
        return m_items;
    }

    uint32_t itemdb::interface__get_hash() const
    {
        return m_hash;
    }

    uint8_t* itemdb::interface__get_data()
    {
        return m_data;
    }

    uint8_t* itemdb::interface__get_packet()
    {
        return m_p_data;
    }

    uint32_t itemdb::interface__get_size() const
    {
        return m_items.size();
    }


    void itemdb::interface__modify_extra_data(const std::string& new_data, bool rehash)
    {
        item* it = nullptr;

        for (auto& item : m_items)
        {
            if (item.m_extra_file == new_data)
            {
                it = &item;
                break;
            }
            else if (item.m_extra_file.empty())
            {
                it = &item;
            }
        }

        if (!it)
            return;


        it->m_extra_file = new_data;
        
        if (rehash)
        {
            uintmax_t filesize;
            void* data = io::read_all_bytes(fmt::format("{}cache/{}", config::settings::fs::data_root, new_data), filesize);

            it->m_extra_file_hash = text::proton::hash(data, filesize);
            free(data);
        }
    }

    uint32_t itemdb::inteface__modify_item(uint32_t id, const beef::item& item)
    {
        beef::item it = m_items.at(id);
        
        if (it.m_name.find("null_item") == std::string::npos && it.m_name != item.m_name)
        {
            m_items.at(id) = item;
            m_items.at(id).m_id = id;

            it.m_id = m_items.size();
            m_items.push_back(it);

            m_item_count += 1;
        }
        else
        {
            m_items.at(id) = item;
            m_items.at(id).m_id = id;
        }


        if (!it.m_texture.empty())
        {
            uintmax_t filesize = 0;
            std::string file = fmt::format("data/cache/game/{}", m_items.at(id).m_texture);

            void* data = NULL;

            if (std::filesystem::exists(file))
                data = io::read_all_bytes(file, filesize);

            if (!data) {}
                // fmt::print("failed to open texture {}\n", it.m_texture);
            else
            {
                m_items.at(id).m_texture_hash = text::proton::hash(data, filesize);
                free(data);
            }
        }

        if (!it.m_extra_file.empty())
        {
            uintmax_t filesize = 0;
            std::string file = fmt::format("data/cache/game/{}", m_items.at(id).m_extra_file);

            void* data = NULL;

            if (std::filesystem::exists(file))
                data = io::read_all_bytes(file, filesize);

            if (!data) {}
                // fmt::print("failed to open {}\n", it.m_texture);
            else
            {
                m_items.at(id).m_extra_file_hash = text::proton::hash(data, filesize);
                free(data);
            }
        }

        return id;
    }

    bool item_is_world_lock(const item& item_)
    {
        return item_.m_id != SMALLLOCK &&
            item_.m_id != BIGLOCK &&
            item_.m_id != HUGELOCK &&
            item_.m_id != BUILDER_SLOCK;
    }
}
