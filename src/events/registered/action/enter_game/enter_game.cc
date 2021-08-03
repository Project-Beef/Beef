#include "enter_game.hh"

#include "constants/dialogs.hh"

namespace beef::events::registered
{
    void enter_game(event_context& ctx)
    {
        ctx.m_player->send_inventory();
        ctx.m_player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", ctx.m_peer->host->connectedPeers));
        
        ctx.m_player->send_variant(variantlist { "OnRequestWorldSelectMenu" });
        ctx.m_player->send_variant(variantlist { "OnDialogRequest", dialogs::gazette });
    }
}