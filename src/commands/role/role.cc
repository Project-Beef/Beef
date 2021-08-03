#include "role.hh"

#include "constants/roles.hh"
#include "database/database.hh"
#include "utils/text.hh"

#include <enet/enet.h>

namespace beef::commands
{
    void role(events::event_context& ctx)
    {
        if (ctx.m_player->m_guest || ctx.m_player->m_current_world == "EXIT" || ctx.m_player->m_admin_level < LVL_DEVELOPER)
        {
            ctx.m_player->send_log("`4Insufficient permissions.``");
            return;
        }

        std::string* str = static_cast<std::string*>(ctx.m_data);

        size_t player_pos = str->find(' ');
        if (player_pos > str->size())
        {
            ctx.m_player->send_log("`4Invalid arguments.``");
            return;
        }

        std::string player_name = str->substr(0, player_pos);
        int level = atoi(str->substr(player_pos + 1).c_str());

        bool found = false;


        if (!text::to_lowercase(player_name))
        {
            ctx.m_player->send_log("`4Invalid name.``");
            return;
        }

        if (player_name == ctx.m_player->m_tank_id)
        {
            ctx.m_player->send_log("Don't even try demoting/promoting yourself.");
        }

        for (ENetPeer* peer = &ctx.m_peer->host->peers[0]; peer < &ctx.m_peer->host->peers[ctx.m_peer->host->peerCount]; ++peer)
        {
            if (peer->state != ENET_PEER_STATE_CONNECTED || !peer->data) continue;

            player* player_ = static_cast<player*>(peer->data);
            if (player_->m_tank_id.empty()) continue;

            if (player_->m_tank_id == player_name)
            {
                player_->m_admin_level = level;

                player_->send_log("You've been `2promoted``.");
                player_->disconnect_peer();

                ctx.m_player->send_log("`2Gave rank`` to an online player.");

                found = true;
                break;
            }
        }

        if (!found)
        {
            player* player_ = new player(nullptr, nullptr);
            std::string file = fmt::format("data/players/_{}.bin", player_name);

            if (std::filesystem::exists(file))
            {
                if (!database::read_player(player_, file))
                {
                    ctx.m_player->send_log("`4No player found with a such name.``");
                }
                else
                {
                    player_->m_admin_level = level;
                    ctx.m_player->send_log("`2Gave rank`` to a offline player.");

                    if (!database::write_player(player_, file))
                    {
                        ctx.m_player->send_log("`4WRITE FAIL!`` Failed to save the player. Please report this for `wfree#1234``.");
                    }
                }
            }
            else
                ctx.m_player->send_log("`4Invalid player.``");

            delete player_;
        }
    }
}
