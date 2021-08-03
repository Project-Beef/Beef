#include "state.hh"
#include "packet/gameupdatepacket.hh"

namespace beef::events::registered
{
    void state(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        ctx.m_game_packet->m_net_id = ctx.m_player->get_net_id();

        ctx.m_player->m_pos.m_x = ctx.m_game_packet->m_pos_x;
        ctx.m_player->m_pos.m_y = ctx.m_game_packet->m_pos_y;

        ctx.m_player->m_rotated_left = ctx.m_game_packet->m_flags & 0x10;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world.`` Please report this for `wfree#1234`` on Discord.");
            return;
        }

        world_->broadcast([&] (int net_id, player* player_) {
            player_->send_packet(4, ctx.m_game_packet, sizeof(gameupdatepacket));
        });
    }
}
