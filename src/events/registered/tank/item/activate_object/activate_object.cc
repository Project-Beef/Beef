#include "activate_object.hh"

namespace beef::events::registered
{
    void item_activate_object(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in) return;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world.``");
            return;
        }

        math::vec2 position = { ctx.m_game_packet->m_pos_x, ctx.m_game_packet->m_pos_y };
        world_->collect_object(ctx.m_player, ctx.m_game_packet->m_object_id, position);
    }
}