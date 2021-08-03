#include "event_manager.hh"

namespace beef::events
{
    void event_manager::load_event(const std::string& name, std::function<void(event_context&)> fn)
    {
        m_events.insert_or_assign(text::quick_hash(name), fn);
    }

    void event_manager::load_event(uint64_t hash, std::function<void(event_context&)> fn)
    {
        m_events.insert_or_assign(hash, fn);
    }

    void event_manager::rem_event(const std::string& name)
    {
        if (auto it = m_events.find(text::quick_hash(name)); it != m_events.end())
            m_events.erase(it);
    }


    bool event_manager::call(const std::string& name, event_context& ctx)
    {
        if (auto it = m_events.find(text::quick_hash(name)); it != m_events.end())
        {
            it->second(ctx);
            return true;
        }

        return false;
    }

    bool event_manager::call(const std::string& name, std::function<void(event_context&)>& fn)
    {
        if (auto it = m_events.find(text::quick_hash(name)); it != m_events.end())
        {
            fn = it->second;
            return true;
        }

        return false;
    }

    bool event_manager::call(uint64_t hash, event_context& ctx)
    {
        if (auto it = m_events.find(hash); it != m_events.end())
        {
            it->second(ctx);
            return true;
        }

        return false;
    }

    const event_manager::value_type& event_manager::get() const
    {
        return m_events;
    }
}
