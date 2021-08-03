#include "action.hh"
#include "events/event_manager.hh"

namespace beef::events::registered
{
    void action(event_context& ctx)
    {
        event_manager* actions = static_cast<event_manager*>(ctx.m_data);

        std::string action;
        if (ctx.m_scanner->try_get("action", action))
        {
            if (!actions->call(action, ctx))
                ctx.m_player->send_log(fmt::format("`wACTION``: Unhandled `w{}``", action));
        }
        else
        {
            ctx.m_player->send_log("`4Invalid data sent to server.");
        }
    }
}
