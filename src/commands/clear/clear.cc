#include "clear.hh"

#include "constants/roles.hh"
#include "enet/enet.h"
#include "itemdb/item_component.hh"
#include "itemdb/item_types.hh"
#include "packet/gameupdatepacket.hh"
#include "world/tile.hh"

namespace beef::commands
{
    void clear(events::event_context& ctx)
    {
        if (ctx.m_player->m_admin_level < LVL_DEVELOPER)
        {
            ctx.m_player->send_log("`4Insufficient permissions.``");
            return;
        }

        if (ctx.m_player->m_current_world == "EXIT")
        {
            ctx.m_player->send_log("You're in EXIT, this is not how it works.");
            return;
        }

        world* world_;
        uintmax_t estimate = 0;

        if (ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            for(auto& tile : world_->m_tiles)
            {
                if (tile.get_fg_uint() == BEDROCK || tile.get_fg_uint() == MAINDOOR) continue;

                tile.set_fg(0);
                tile.set_bg(0);

                tile.remove_flag(TILEFLAG_TILEEXTRA);
                tile.estimate_memory_usage(estimate); // follow 
            }
        }

        std::string world = ctx.m_player->m_current_world;


        gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + (8 * estimate) + 4));
        if (!packet)
            return;

        packet->m_type = PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE;
        packet->m_flags = 8;
        packet->m_int_x = -1;
        packet->m_int_y = -1;

        uintmax_t pos = 0;

        for (size_t i = 0; i < world_->m_tiles.size(); ++i)
        {
            int x = static_cast<int>(i) % world_->m_width, y = static_cast<int>(i) / world_->m_width;

            memcpy(&packet->m_data + pos, &x, 4);
            memcpy(&packet->m_data + pos + 4, &y, 4);
            pos += 8;
            
            world_->m_tiles[i].pack(&packet->m_data, pos);
        }

        *reinterpret_cast<int*>(&packet->m_data + pos) = -1;
        pos += 4;

        packet->m_data_size = pos;
        ctx.m_player->send_log("Cleared the world successfully.");

        world_->broadcast([&] (int net_id, player* player_) {
            player_->send_log(fmt::format("World was cleared by {}.", ctx.m_player->m_display_name));
            player_->send_packet(4, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
        });

        free(packet);

        /*for (ENetPeer* peer = ctx.m_peer->host->peers; peer < &ctx.m_peer->host->peers[ctx.m_peer->host->peerCount]; ++peer)
        {
            if (peer->state != ENET_PEER_STATE_CONNECTED) continue;

            if (peer->data)
            {
                player* player_ = static_cast<player*>(peer->data);
                if (player_->m_current_world != world) continue;

                if (player_ == ctx.m_player)
                    player_->send_log(fmt::format("Successfully cleared world `w{}``.", world));
                else
                    player_->send_log(fmt::format("World was `2cleared`` by {}.", ctx.m_player->m_display_name));


                //ctx.m_manager->quit_to_exit(player_);
                //ctx.m_manager->join_world(player_, world);
            }*/
        //}
    }
}
