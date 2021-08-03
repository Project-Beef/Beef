#include "icon_state.hh"

namespace beef::events::registered
{
    void icon_state(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        ctx.m_game_packet->m_net_id = ctx.m_player->get_net_id();

        world_->broadcast([&] (int net_id, player* player_) {
            player_->send_packet(4, ctx.m_game_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
        });
    }
}