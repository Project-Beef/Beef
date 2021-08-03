#include "respawn_spike.hh"

namespace beef::events::registered
{
    void respawn_spike(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        world_->respawn_player(ctx.m_player, true);
    }
}