#ifndef __EVENTS__EVENT_CONTEXT__HH_
#define __EVENTS__EVENT_CONTEXT__HH_

#include "packet/gameupdatepacket.hh"
#include "world/world_manager.hh"
#include "player/player.hh"

#include <enet/enet.h>

namespace beef::events
{
    struct event_context
    {
        ENetPacket* m_packet;
        ENetPeer* m_peer;

        player* m_player;
        server* m_server;
        world_manager* m_manager;

        text::scanner* m_scanner; // empty when packet type != 2 or 3
        gameupdatepacket* m_game_packet; // empty when packet type != 4

        void* m_data; // freely modifiable data
        void* m_data2; // freely modifiable data
    };
}

#endif // __EVENTS__EVENT_CONTEXT__HH_
