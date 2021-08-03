#ifndef __WORLD__WORLD_OBJECT__HH__
#define __WORLD__WORLD_OBJECT__HH__

#include "math/vec2.hh"

namespace beef
{
    enum
    {
        OBJECT_CHANGE_TYPE_COLLECT = 0,
        OBJECT_CHANGE_TYPE_ADD = -1,
        OBJECT_CHANGE_TYPE_REMOVE = -2,
        OBJECT_CHANGE_TYPE_MODIFY = -3,
    };

    struct world_object
    {
        short m_item_id = 0;
        uint8_t m_flags = 0;
        uint8_t m_item_amount = 0;
        
        math::vec2 m_pos = { 0, 0 };
    };
}

#endif