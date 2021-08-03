#include "world_manager.hh"

#include "math/vec2.hh"
#include "utils/text.hh"
#include "constants/roles.hh"
#include "database/database.hh"
#include "itemdb/item_component.hh"
#include "packet/gameupdatepacket.hh"

#include <fmt/core.h>
#include <fmt/compile.h>

#include <enet/enet.h>

namespace beef
{
    world_manager::world_manager()
    {
    }


    bool world_manager::join_world(player* invoker, const std::string& name)
    {
        std::string file = fmt::format("data/worlds/_{}.bin", name);

        world* world_ = nullptr;

        if (!get_world(name, &world_))
        {
            world_ = new world(name);

            if (std::filesystem::exists(file))
            {
                database::read_world(world_, file);
                uint64_t hash = text::quick_hash(name);

                m_worlds[hash] = world_;
            }
            else
            {
                world_->generate_new_world();
                uint64_t hash = text::quick_hash(name);

                m_worlds[hash] = world_;
            }
        }

        // world_->m_name = name;
        world_->add_player(invoker);


        gameupdatepacket* packet = world_->pack();
        if (!packet)
            return false;

        invoker->send_packet(4, packet, packet->m_data_size + sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
        free(packet);

        math::vec2i door = world_->get_tile_pos(MAINDOOR);

        invoker->m_pos.m_x = door.m_x * 32;
        invoker->m_pos.m_y = door.m_y * 32;

        invoker->send_variant(variantlist {
            "OnSpawn",
            fmt::format(FMT_COMPILE(
                "spawn|avatar\n"
                "netID|{0}\n"
                "userID|{3}\n"
                "colrect|0|0|20|30\n"
                "posXY|{4}|{5}\n"
                "name|{1}\n"
                "country|{2}\n"
                "invis|0\n"
                "mstate|0\n"
                "smstate|1\n"
                "onlineID|\n"
                "type|local\n"),

                invoker->get_net_id(),
                invoker->m_display_name,
                invoker->m_country,
                invoker->get_user_id(),
                invoker->m_pos.m_x, invoker->m_pos.m_y
            )
        });

        invoker->send_variant(variantlist {
            "OnSetClothing",
            math::vec3(invoker->m_cloth.m_hair, invoker->m_cloth.m_shirt, invoker->m_cloth.m_pants),
            math::vec3(invoker->m_cloth.m_feet, invoker->m_cloth.m_face, invoker->m_cloth.m_hand),
            math::vec3(invoker->m_cloth.m_back, invoker->m_cloth.m_mask, invoker->m_cloth.m_necklace),
            invoker->m_skin_color,
            math::vec3(invoker->m_cloth.m_ances, 0, 0)
        }, invoker->get_net_id());
/*
        gameupdatepacket p;
        p.m_type = PACKET_SET_CHARACTER_STATE;
        
        p.m_count1 = 128;
        p.m_count2 = 128;
        p.m_net_id = invoker->get_net_id();
        
        p.m_int_data = 1 << 1 | 1 << 24;

        p.m_speed_in = 300;
        p.m_speed_out = 600;

        p.m_tiles_length = 1000;
        p.m_flags = 100;

        p.m_water_speed = 125.f;

        gameupdatepacket* pp = &p;

        invoker->send_packet(4, &p, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
*/

        /*
        flags |= m_noclip << STATE_FLAGS_NOCLIP;
    flags |= m_wings << STATE_FLAGS_DOUBLE_JUMP;
    flags |= m_invis << STATE_FLAGS_INVISIBLE;
    flags |= m_nohand << STATE_FLAGS_NO_HAND;
    flags |= m_noeye << STATE_FLAGS_NO_EYE;
    flags |= m_nobody << STATE_FLAGS_NO_BODY;
    flags |= m_devilhorn << STATE_FLAGS_DEVIL_HORN;
    flags |= m_goldenhalo << STATE_FLAGS_GOLDEN_HALO;
    flags |= m_frozen << STATE_FLAGS_FROZEN;
    flags |= m_cursed << STATE_FLAGS_CURSED;
    flags |= m_ducttaped << STATE_FLAGS_DUCT_TAPE;
    flags |= m_cigar << STATE_FLAGS_CIGAR;
    flags |= m_shining << STATE_FLAGS_SHINING;
    flags |= m_zombie << STATE_FLAGS_ZOMBIE;
    flags |= m_redbody << STATE_FLAGS_RED_BODY;
    flags |= m_hauntedshadows << STATE_FLAGS_HAUNTED_SHADOWS;
    flags |= m_geigerradiation << STATE_FLAGS_GEIGER_RADIATION;
    flags |= m_spotlight << STATE_FLAGS_SPOTLIGHT;
    flags |= m_yellowbody << STATE_FLAGS_YELLOW_BODY;
    flags |= m_pineapple << STATE_FLAGS_PINEAPPLE;
    flags |= m_flyingpineapple << STATE_FLAGS_FLYING_PINEAPPLE;
    flags |= m_supporter << STATE_FLAGS_SUPPORTER_NAME;
    flags |= m_superpineapple << STATE_FLAGS_SUPPER_PINEAPPLE;*/

        gameupdatepacket state;

        state.m_type = PACKET_SET_CHARACTER_STATE;

        state.m_build_range = 0x80;
        state.m_punch_range = 0x80;
        state.m_punch_id = 0; // todo

        state.m_net_id = invoker->get_net_id();
        state.m_accel = 1200.f;
        state.m_effect_flags = invoker->m_admin_level >= LVL_MODERATOR ? 1 << 0x18 | 1 << 1 | 1 << 0 : 0; //m_state.GetFlag();
        state.m_velocity_x = 250.f;

        /*
        if (m_player->m_bodyPart.IsWearing(Item::Zamgrowni) ||
        m_player->m_bodyPart.IsWearing(Item::Harvester) ||
        m_player->m_bodyPart.IsWearing(Item::PineapplePlough) ||
        m_player->m_bodyPart.IsWearing(Item::DearJohnTractor) ||
        m_player->m_bodyPart.IsWearing(Item::HarvesterOfSorrows))
    {
        result = 150;
    }

    if (m_coffee)
        result += 120;
    if (m_speedy)
        result += 60;*/

        state.m_velocity_y = 1000.f;
        /*
            if (m_floating)
    {
        result = -20;
    }
    else if (m_boostJump)
    {
        result = 700;
    }*/

        state.m_water_speed = 125.f;
        state.m_punch_strength = 250.f;

        invoker->send_packet(4, &state, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);

        world_->broadcast([&] (int net_id, player* player_) {
            if (player_ == invoker) return;

            gameupdatepacket player_state = state;
            player_state.m_net_id = player_->get_net_id();

            player_->send_packet(4, &state, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
            invoker->send_packet(4, &player_state, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);

            player_->send_variant(variantlist {
                "OnSpawn",
                fmt::format(FMT_COMPILE(
                    "spawn|avatar\n"
                    "netID|{0}\n"
                    "userID|{3}\n"
                    "colrect|0|0|20|30\n"
                    "posXY|{4}|{5}\n"
                    "name|{1}\n"
                    "country|{2}\n"
                    "invis|0\n"
                    "mstate|0\n"
                    "smstate|1\n"
                    "onlineID|\n"),
                    invoker->get_net_id(),
                    invoker->m_display_name,
                    invoker->m_country,
                    invoker->get_user_id(),
                    invoker->m_pos.m_x, invoker->m_pos.m_y
                )
            });

            player_->send_cloth(invoker);

            /*player_->send_variant(variantlist {
                "OnSetClothing",
                math::vec3(invoker->m_cloth.m_hair, invoker->m_cloth.m_shirt, invoker->m_cloth.m_pants),
                math::vec3(invoker->m_cloth.m_feet, invoker->m_cloth.m_face, invoker->m_cloth.m_hand),
                math::vec3(invoker->m_cloth.m_back, invoker->m_cloth.m_mask, invoker->m_cloth.m_necklace),
                invoker->m_skin_color,
                math::vec3(invoker->m_cloth.m_ances, 0, 0)
            }, invoker->get_net_id());*/

            invoker->send_variant(variantlist {
                "OnSpawn",
                fmt::format(FMT_COMPILE(
                    "spawn|avatar\n"
                    "netID|{0}\n"
                    "userID|{3}\n"
                    "colrect|0|0|20|30\n"
                    "posXY|{4}|{5}\n"
                    "name|{1}\n"
                    "country|{2}\n"
                    "invis|0\n"
                    "mstate|0\n"
                    "smstate|1\n"
                    "onlineID|\n"),
                    player_->get_net_id(),
                    player_->m_display_name,
                    player_->m_country,
                    player_->get_user_id(),
                    player_->m_pos.m_x, player_->m_pos.m_y
                )
            });

            invoker->send_cloth(player_);
        });

        return true;
    }

    bool world_manager::quit_to_exit(player* invoker, bool disconnected)
    {
        world* world_;
        if (get_world(invoker->m_current_world, &world_))
        {
            world_->remove_player(invoker->get_net_id());

            if (world_->m_players.size() == 0)
            {
                database::write_world(world_, fmt::format("data/worlds/_{}.bin", world_->m_name));


                auto it = std::find_if(m_worlds.begin(), m_worlds.end(),
                        [&world_](const auto & p) { return p.second->m_name == world_->m_name; });

                if (it != m_worlds.end())
                {
                    m_worlds.erase(it);
                }

                delete world_;
            }
            else
            {
                world_->broadcast([&] (int net_id, player* player_) {
                    if (player_ == invoker) return;

                    player_->send_variant(variantlist {
                        "OnRemove",
                        fmt::format(
                            "netID|{}",
                            invoker->get_net_id()
                        )
                    });
                });
            }

            if (!disconnected)
            {
                invoker->send_variant(variantlist { "OnRequestWorldSelectMenu" });
            }

            invoker->m_current_world = "EXIT";
        }

        return true;
    }

    void world_manager::save_all_worlds()
    {
        for (auto& world : m_worlds)
        {
            if (!database::write_world(world.second, fmt::format("data/worlds/_{}.bin", world.second->m_name)))
                fmt::print("failed to save world {}\n", world.second->m_name);

            delete world.second;
            m_worlds.erase(world.first);
        }
    }


    bool world_manager::get_world(const std::string& name, world** world_)
    {
        if (auto it = m_worlds.find(text::quick_hash(name)); it != m_worlds.end())
        {
            if (!it->second) return false;

            *world_ = it->second;
            return true;
        }

        return false;
    }

    bool world_manager::rejoin_world(player* invoker)
    {
        return false;
    }
}
