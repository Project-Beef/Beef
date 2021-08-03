#include "join_request.hh"

#include "utils/text.hh"

namespace beef::events::registered
{
    void join_request(event_context& ctx)
    {
        if (!(ctx.m_scanner->try_get("name", ctx.m_player->m_current_world) && text::to_uppercase(ctx.m_player->m_current_world)))
        {
            ctx.m_player->send_log("`4Invalid world name.");
            ctx.m_player->send_variant(variantlist { "OnFailedToEnterWorld", 1 });
            return;
        }

        ctx.m_manager->join_world(ctx.m_player, ctx.m_player->m_current_world);
    }
}