#include "quit.hh"

namespace beef::events::registered
{
    void quit(event_context& ctx)
    {
        ctx.m_player->disconnect_peer();
    }
}
