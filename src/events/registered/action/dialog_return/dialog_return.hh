#ifndef __EVENTS_REGISTERED__ACTION__DIALOG_RETURN__HH__
#define __EVENTS_REGISTERED__ACTION__DIALOG_RETURN__HH__

#include "events/event_context.hh"

namespace beef::events::registered
{
    extern std::vector<std::string> cache; // to free, to make it faster - kevz :D
    void dialog_return(event_context& ctx);
}

#endif // __EVENTS_REGISTERED__ACTION__DIALOG_RETURN__HH__
