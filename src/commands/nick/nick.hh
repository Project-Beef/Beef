#ifndef __COMMANDS__NICK__NICK__HH__
#define __COMMANDS__NICK__NICK__HH__

#include "events/event_context.hh"

namespace beef::commands
{
    void nick(events::event_context& ctx);
}

#endif // __COMMANDS__NICK__NICK__HH__