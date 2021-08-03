#include "get.hh"
#include "constants/roles.hh"

namespace beef::commands
{
    void get(events::event_context& ctx)
    {
        std::string* str = static_cast<std::string*>(ctx.m_data);
        if (str->empty())
        {
            ctx.m_player->send_log("`4Invalid id for an item.``");
            return;
        }

        int item = atoi(str->c_str());
        if (item > itemdb::get_size() || item < 1)
        {
            ctx.m_player->send_log("`4Trying to get non-existing item, huh?``");
            return;
        }

        if ((item == MAINDOOR || item == BEDROCK) && ctx.m_player->m_admin_level < LVL_MANAGER) return;

        ctx.m_player->add_inventory_item(static_cast<uint16_t>(item), 200, true);
    }
}
