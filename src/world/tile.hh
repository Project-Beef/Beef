#ifndef __WORLD__WORLD_TILE__HH__
#define __WORLD__WORLD_TILE__HH__

#include <chrono>
#include <stdint.h>

#include "math/vec2i.hh"

#include "itemdb/item.hh"
#include "itemdb/itemdb.hh"
#include "itemdb/item_flags.hh"
#include "itemdb/item_component.hh"

#include "utils/time.hh"

namespace beef
{
    enum
    {
        TILEFLAG_TILEEXTRA = 1 << 0,
        TILEFLAG_LOCKED = 1 << 1,
        TILEFLAG_SEED = 1 << 4,
        TILEFLAG_FLIPPED = 1 << 5,
        TILEFLAG_OPEN = 1 << 6,
        TILEFLAG_PUBLIC = 1 << 7,
        TILEFLAG_SILENCED = 1 << 9,
        TILEFLAG_WATER = 1 << 10,
        TILEFLAG_FIRE = 1 << 12,
        TILEFLAG_RED = 1 << 13,
        TILEFLAG_BLUE = 1 << 14,
        TILEFLAG_GREEN = 1 << 15,
    };

    class world;

    class tile
    {
    public:
        tile();
        tile(uint16_t fg, uint16_t bg, uint16_t lp, uint16_t fl);

        void set_fg(uint16_t id);
        void set_bg(uint16_t id);
        void set_world(world* world_);

        uint16_t get_base_uint() const;
        const item& get_base_item() const;

        void reset_hit();
        void remove_base();
        void remove_lock();

        uint16_t get_fg_uint() const;
        uint16_t get_bg_uint() const;
        uint8_t indicate_hit();

        void set_locked(bool locked);
        bool is_locked() const;
        
        int get_owner_id() const;
        uint8_t get_hit_count() const;
        time_point get_last_hit_time() const;

        uint8_t get_display_item() const;
        void set_display_item(uint16_t id);

        void reset_flag();
        void set_flag(uint16_t flags);
        void remove_flag(uint16_t flags);

        void set_new_seed();
        void set_new_lock(int user_id = -1);
        void set_new_lock(int user_id, const math::vec2i& parent);

        uint8_t get_fruit_count() const;
        time_point get_last_planted() const;

        void set_label(const std::string& label);
        void remove_label();
        const std::string& get_label() const;

        void pack(uint8_t* data, uintmax_t& pos);
        size_t estimate_memory_usage(size_t& consum) const;

        uint16_t get_lp() const;
        uint16_t get_fl() const;

    public:
        friend void handle_tile_extra(uint8_t* data, uintmax_t& pos, const tile* tile, bool to_database);
        friend void read_tile_extra(uint8_t* data, uintmax_t& pos, tile* tile);

    private:
        bool m_locked = false;
        bool m_spliced = false;

        uint16_t m_fg;
        uint16_t m_bg;
        uint16_t m_lp;
        uint16_t m_fl;
        uint16_t m_display_item = 0;

        uint8_t m_hit_count = 0;
        uint8_t m_lock_flags = 0;
        uint8_t m_fruit_count = 0;

        int m_owner = -1;

        time_point m_last_hitten {};
        time_point m_last_planted {};

        std::string m_label;
        std::vector<int> m_access;

        world* m_world = NULL;
    };

    void handle_tile_extra(uint8_t* data, uintmax_t& pos, const tile* tile, bool to_database);
    void read_tile_extra(uint8_t* data, uintmax_t& pos, tile* tile);
}

#endif // __WORLD__WORLD_TILE__HH__
