#ifndef __EVENTS__EVENT_MANAGER__HH__
#define __EVENTS__EVENT_MANAGER__HH__

#include <enet/enet.h>

#include <string>
#include <functional>
#include <unordered_map>

#include "utils/text.hh"

#include "events/event_context.hh"

namespace beef::events
{
    class event_manager
    {
    public:
        using value_type = std::unordered_map<uint64_t, std::function<void(event_context&)>>;

        event_manager() = default;

        void load_event(const std::string& name, std::function<void(event_context&)> fn);
        void load_event(uint64_t hash, std::function<void(event_context&)> fn);
        void rem_event(const std::string& name);

        bool call(const std::string& name, event_context& ctx);
        bool call(const std::string& name, std::function<void(event_context&)>& fn);
        bool call(uint64_t hash, event_context& ctx);

        const value_type& get() const;

    private:
        value_type m_events;
    };
}

#endif // __EVENTS__EVENT_MANAGER__HH__
