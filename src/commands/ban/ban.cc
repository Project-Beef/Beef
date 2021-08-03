#include "ban.hh"

#include "constants/roles.hh"

#include "packet/gameupdatepacket.hh"
#include "packet/variant.hh"
#include "utils/text.hh"
#include "utils/time.hh"
#include <ctime>

namespace beef::commands
{
    void ban(events::event_context& ctx)
    {
        //if (ctx.m_player->m_current_world == "EXIT" || ctx.m_player->m_admin_level <= LVL_MODERATOR || !ctx.m_player->m_logged_in) return;

        std::string& name = *static_cast<std::string*>(ctx.m_data);
        if (!text::to_lowercase(name))
        {
            ctx.m_player->send_log("`4Invalid name``");
            return;
        }

        player* player_ = ctx.m_server->get_player_by_name(name);
        if (!player_)
        {
            ctx.m_player->send_log("`4Player not found`` (if they're offline, dm `wfree#1234``)");
            return;
        }

        ctx.m_player->send_log("`4BANNINGGGGGGGGGGGG``");

        player_->ban(std::chrono::seconds(60), ctx.m_server);
    }
}
