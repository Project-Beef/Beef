#include "quit_to_exit.hh"

namespace beef::events::registered
{
    void quit_to_exit(event_context& ctx)
    {
        ctx.m_manager->quit_to_exit(ctx.m_player, false);
        
        ctx.m_player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", ctx.m_peer->host->connectedPeers));
    }
}