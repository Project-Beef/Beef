#ifndef __ALGORITHM__LOCK__HH__
#define __ALGORITHM__LOCK__HH__

#include "world/world.hh"
#include "player/player.hh"

namespace beef::algorithm::lock
{
	void apply(player* player_, world* world_, gameupdatepacket* packet, bool reapply);
}

#endif // __ALGORITHM__LOCK__HH__