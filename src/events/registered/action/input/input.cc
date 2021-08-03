#include "input.hh"

#include <enet/enet.h>

#include "events/event_context.hh"
#include "events/event_manager.hh"
#include "packet/gameupdatepacket.hh"
#include "utils/text.hh"

namespace beef::events::registered
{
    void input(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        std::string text;
        if (!ctx.m_scanner->try_get("|text", text))
        {
            ctx.m_player->send_log("`4Invalid text received.`` Please report this to `wfree#1234``.");
            return;
        }

        text::trim(text);
        if (text.empty()) return;

        if (text[0] == '/')
        {
            event_manager* commands = static_cast<event_manager*>(ctx.m_data2);

            std::string command = text.substr(1, text.find(' ') - 1);

            std::string args = "";
            if (2 + command.size() < text.size())
                args = text.substr(2 + command.size());

            std::function<void(event_context& ctx)> function;

            if (!commands->call(command, function))
                ctx.m_player->send_log("`4Unknown command.``");
            else
            {
                ctx.m_player->send_log(fmt::format("`6{}``", text));
                ctx.m_data = &args;

                function(ctx);
            }
        }
        else
        {
            world* world_;
            if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
            {
                ctx.m_player->send_log("`wWORLD``: `4Invalid world.``");
                return;
            }

            gameupdatepacket* p1 = variantlist { "OnConsoleMessage", fmt::format("CP:0_PL:4_OID:_CT:[W]_ `o<`w{}``> `{}{}``", ctx.m_player->m_display_name, ctx.m_player->m_admin_level > LVL_BETA_TESTER ? ctx.m_player->m_chat_color : 'o', text) }.pack();
            gameupdatepacket* p2 = variantlist { "OnTalkBubble", ctx.m_player->get_net_id(), fmt::format("`{}{}``", ctx.m_player->m_chat_color, text), 0 }.pack();

            p1->m_net_id =-1;
            p1->m_delay = -1;

            p2->m_net_id = -1;
            p2->m_delay = -1;

            world_->broadcast([&] (int net_id, player* player_) {
                player_->send_packet(4, p1, sizeof(gameupdatepacket) + p1->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                player_->send_packet(4, p2, sizeof(gameupdatepacket) + p2->m_data_size, ENET_PACKET_FLAG_RELIABLE);
            });

            free(p1);
            free(p2);
        }
    }
}