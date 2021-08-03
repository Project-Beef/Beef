#ifndef __WORLD__WORLD__HH__
#define __WORLD__WORLD__HH__

#include "tile.hh"
#include "world_object.hh"

#include "utils/rng.hh"
#include "math/vec2i.hh"
#include "player/player.hh"
#include "database/database.hh"
#include "packet/gameupdatepacket.hh"

#include <string>
#include <algorithm>
#include <functional>
#include <filesystem>

#include <fmt/core.h>

namespace beef
{
    class world
    {
    public:
        world();
        world(const std::string& name);

        void add_tile(const tile& tile);
        tile* get_tile(int x, int y);
        tile* get_tile(const math::vec2i& pos);

        bool is_locked() const;
        bool is_owner(player* player_) const;
        bool is_admin(player* player_) const;

        const tile& get_c_tile(uint32_t index) const;
        math::vec2i get_tile_pos(uint32_t id) const;
        math::vec2i get_tile_pos_by_type(uint32_t type) const;

        size_t estimate_memory() const;

        gameupdatepacket* pack();

        void set_new_lock(int user_id = -1);
        void generate_new_world(int size = 6000);

        void broadcast(std::function<void(int, player*)> cb);

        void add_object(world_object& object, bool check);
        void remove_object(uint32_t id);
        void modify_object(const std::pair<uint32_t, world_object>& object);
        void collect_object(player* player_, uint32_t id, const math::vec2& position);
        void add_gems_object(int gems, const math::vec2& position);

        void add_player(player* player_);
        void respawn_player(player* player_, bool killed, int delay = -1);
        void remove_player(int net_id);

    public:
        std::string m_name;

        uint8_t m_lock_flags = 0;

        int m_tempo = 100;
        int m_width = 100;
        int m_height = 60;

        int m_owner = -1;
        int m_net_id = 1;
        int m_owner_id = -1;
        int m_object_id = 1;

        std::vector<int> m_access;
        std::vector<tile> m_tiles;
        std::unordered_map<int, player*> m_players;
        std::unordered_map<int, world_object> m_objects;
    };
}

#endif // __WORLD__WORLD__HH__
