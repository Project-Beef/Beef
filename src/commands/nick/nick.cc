#include "nick.hh"
#include "constants/roles.hh"
#include "enet/enet.h"
#include "packet/gameupdatepacket.hh"
#include "packet/variant.hh"

namespace beef::commands
{
    void nick(events::event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || ctx.m_player->m_admin_level < LVL_MODERATOR)
        {
            ctx.m_player->send_log("`4Insufficient permissions.``");
            return;
        }

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        std::string* name = static_cast<std::string*>(ctx.m_data);
        if (name->empty() || name->find("`6") != std::string::npos || name->find("of Legend") != std::string::npos || name->find("Dr.") != std::string::npos)
        {
            ctx.m_player->send_log("`4Invalid name``");
            return;
        }

        gameupdatepacket* packet = variantlist { "OnNameChanged", fmt::format("`w{}``", name->c_str()) }.pack();
        packet->m_net_id = ctx.m_player->get_net_id();

        ctx.m_player->send_log("You're `wnicked`` now. Please, don't abuse this feature - might lead into permament demotion, if abused.");

        world_->broadcast([&] (int net_id, player* player_) {
            player_->send_packet(4, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
        });

        free(packet);
    }
}
