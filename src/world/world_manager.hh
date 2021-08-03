#ifndef __WORLD__WORLD_MANAGER__HH__
#define __WORLD__WORLD_MANAGER__HH__

#include "world.hh"
#include "utils/text.hh"
#include "player/player.hh"

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace beef
{
    class world_manager
    {
    public:
        world_manager();

        bool join_world(player* invoker, const std::string& name);
        bool quit_to_exit(player* invoker, bool disconnected = false);

        void save_all_worlds();

        bool get_world(const std::string& name, world** world_);
        bool rejoin_world(player* invoker);
        // void get_random_worlds(std::vector<world>& worlds);

    private:
        std::unordered_map<uint64_t, world*> m_worlds;
    };
}

#endif // __WORLD__WORLD_MANAGER__HH__
