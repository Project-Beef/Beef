#ifndef __EVENTS__REGISTERED__TANK__TILE__CHANGE__CHANGE__HH__
#define __EVENTS__REGISTERED__TANK__TILE__CHANGE__CHANGE__HH__

#include "events/event_context.hh"

namespace beef::events::registered
{
    void tile_change(event_context& ctx);
    void add_objects_when_breaking_block(player* player_, world* world_, tile* tile_, const math::vec2& position); 
    void add_objects_when_breaking_tree(player* player_, world* world_, tile* tile_, const math::vec2& position);
}

#endif // __EVENTS__REGISTERED__TANK__TILE__CHANGE__CHANGE__HH__
