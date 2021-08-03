#ifndef __DATABASE__DATABASE__HH__
#define __DATABASE__DATABASE__HH__

#include <string_view>
#include <memory>

#include "utils/io.hh"
#include "world/tile.hh"
#include "server/server.hh"
#include "constants/config.hh"
#include "itemdb/item_flags.hh"

namespace beef
{
    class world;
    class player;

    namespace database
    {
        bool read_user_id(server* server) noexcept;
        void write_user_id(server* server) noexcept;

        bool read_player(player* player_, const std::string_view& file) noexcept;
        bool write_player(const player* player_, const std::string_view& file) noexcept;

        bool read_world(world* world_, const std::string_view& file) noexcept;
        bool write_world(const world* world_, const std::string_view& file) noexcept;
    }
}

#endif // __DATABASE__DATABASE__HH__
