#include "growid.hh"
#include "constants/roles.hh"
#include "database/database.hh"
#include "utils/text.hh"
#include "utils/time.hh"

namespace beef::events::registered
{
    void growid(event_context& ctx)
    {
        if (
            !(
                ctx.m_player->get_info(ctx.m_scanner) &&
                ctx.m_player->get_info_tank(ctx.m_scanner)
            )
        )
        {
            ctx.m_player->disconnect_peer();
        }

        if (ctx.m_player->m_version <= ('3' << 16 | '6' << 8 | '2'))
        {
            ctx.m_player->send_log("`4UPDATE REQUIRED!``: You're on incomptaible version `${}``. It's unsupported, and may lead into crashes.");
            ctx.m_player->send_text("action|set_url\nurl|https://discord.gg/YkScr8w5pg\nlabel|Join our Discord server");
            return;
        }

        if (!text::to_lowercase(ctx.m_player->m_tank_id))
        {
            ctx.m_player->send_log("`4Invalid characters in the name. Please remove them.");
            return;
        }

        std::string file = fmt::format("data/players/_{}.bin", ctx.m_player->m_tank_id);

        if (!std::filesystem::exists(file))
        {
            ctx.m_player->send_log("`4Invalid username or password.");
            return;
        }

        if (!database::read_player(ctx.m_player, file))
        {
            ctx.m_player->send_log("`4Invalid username or password.");
            return;
        }

        if (ctx.m_player->m_tank_pass != ctx.m_player->m_raw_pass)
        {
            ctx.m_player->send_log("`4Invalid username or password.");
            return;
        }

        if (ctx.m_player->m_banned_time != std::chrono::seconds(0))
        {
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - ctx.m_player->m_banned_at);

            if (diff < ctx.m_player->m_banned_time)
            {
                std::string time_message = time::to_string(ctx.m_player->m_banned_time - diff);
                ctx.m_player->send_log(fmt::format(
                        "`4Sorry, this account, device or location has been temporarily banned. "
                        "`oIf you didn't do anything wrong, it could be because you're playing from the same place or on the same device as someone who did. "
                        "Contact one of the `5staffs`` `oin discord if you have any questions. "
                        "This is temporary ban and will be removed in `w{}`o. "
                        "If that's not your name, try playing from another location or device to fix it.",
                        time_message
                    )
                );
                ctx.m_player->m_banned_time = {};
                ctx.m_player->send_text("action|set_url\nurl|https://discord.gg/beefps\nlabel|`1Join Beef's Discord Server\n");
                ctx.m_player->send_text("action|logon_fail\n");
                ctx.m_player->disconnect_peer();
                return;
            }
        }

        if (ctx.m_player->m_admin_level >= LVL_NORMAL)
        {
            ctx.m_player->set_display_name_and_chat_color();
        }

        auto time = std::chrono::system_clock::now() - ctx.m_server->get_uptime();

        auto hours = std::chrono::duration_cast<std::chrono::hours>(time).count();
        auto mins = std::chrono::duration_cast<std::chrono::minutes>(time).count() % 60;
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(time).count() % 60;

        ctx.m_player->send_log(fmt::format("Welcome to `9beef``, {}. Uptime is currently `w{} hours``, `w{} minutes`` and `w{} seconds``", ctx.m_player->m_display_name, hours, mins, secs));
        ctx.m_player->send_main();

        ctx.m_player->m_logged_in = true;

        ctx.m_server->add_player(ctx.m_player);
    }
}