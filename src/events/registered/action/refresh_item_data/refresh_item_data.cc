#include "refresh_item_data.hh"

namespace beef::events::registered
{
    void refresh_item_data(event_context& ctx)
    {
        ctx.m_player->send_log("One moment, updating item data...");
        ctx.m_player->send_packet(itemdb::get_packet(), 4 + 56 + reinterpret_cast<gameupdatepacket*>(itemdb::get_packet() + 4)->m_data_size, ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
    }
}
