#include "help.hh"

#include "events/event_manager.hh"

namespace beef::commands
{
    void help(events::event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT") return;

        std::string str = ">>> Commands: /help, /clear, /get, /nick, /role";
        ctx.m_player->send_log(str);

        /*events::event_manager* manager = static_cast<events::event_manager*>(ctx.m_data2);
        for (const auto& command : manager->get())
        {
            text::quick_hash(command.first);
        }*/
    }
}
