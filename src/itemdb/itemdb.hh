#ifndef __ITEMDB__ITEMDB__HH__
#define __ITEMDB__ITEMDB__HH__

#include <vector>
#include <filesystem>

#include <fmt/core.h>

#include "item.hh"
#include "item_component.hh"
#include "packet/gameupdatepacket.hh"

namespace beef
{
    class itemdb
    {
    public:
        static void kill() { get().interface__kill(); }
        static void init(const std::filesystem::path& path) { get().interface__init(path); }
        static void encode(const std::filesystem::path& path) { get().interface__encode(path); }
        static void rehash() { get().interface__rehash(); }

        static itemdb& get() { static itemdb items; return items; }

        static const item& get_item(uint32_t id) { return get().interface__get_item(id); }
        static item& get_item_unsafe(uint32_t id) { return get().interface__get_item_unsafe(id); }

        static const item& get_item(const std::string& name) { return get().interface__get_item(name); }
        static std::vector<item> get_items_matching(const std::string& name) { return get().interface__get_items_matching(name); }

        static uint32_t get_hash() { return get().interface__get_hash(); }
        static uint8_t* get_data() { return get().interface__get_data(); }
        static uint8_t* get_packet() { return get().interface__get_packet(); }
        static uint32_t get_size() { return get().interface__get_size(); }

        static void modify_extra_data(const std::string& new_data, bool rehash = true) { get().interface__modify_extra_data(new_data, rehash); }
        static uint32_t modify_item(uint32_t id, const beef::item& item) { return get().inteface__modify_item(id, item); }

    private:
        itemdb() = default;
        ~itemdb();

        void interface__init(const std::filesystem::path& path);
        void interface__encode(const std::filesystem::path& path);
        void interface__kill();
        void interface__rehash();
        
        const item& interface__get_item(uint32_t id) const;
        item& interface__get_item_unsafe(uint32_t id);

        const item& interface__get_item(const std::string& name) const;
        std::vector<item> interface__get_items_matching(const std::string& name) const;

        const std::vector<item>& interface__get_items();

        uint32_t interface__get_hash() const;
        uint8_t* interface__get_data();
        uint8_t* interface__get_packet();
        uint32_t interface__get_size() const;

        void interface__modify_extra_data(const std::string& new_data, bool rehash = true);
        uint32_t inteface__modify_item(uint32_t id, const beef::item& item);
    
    private:
        std::vector<item> m_items;

        uint32_t m_hash;
        uint8_t* m_data;
        uint8_t* m_p_data;

        uint32_t m_item_count;
        uint16_t m_version;
    };

    bool item_is_world_lock(const item& item_);
}

#endif // __ITEMDB__ITEMDB__HH__
