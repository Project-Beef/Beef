#include "activate.hh"

#include "itemdb/item_types.hh"

namespace beef::events::registered
{
    void tile_activate(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in) return;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        tile* ti = world_->get_tile(ctx.m_game_packet->m_int_x, ctx.m_game_packet->m_int_y);
        const item& it = itemdb::get_item(ti->get_base_uint());

        switch (it.m_action_type)
        {
            case ITEMTYPE_MAIN_DOOR:
            {
                ctx.m_manager->quit_to_exit(ctx.m_player);
                break;
            }
            case ITEMTYPE_DOOR:
            {
                break;
            }
        }
    }
}