#ifndef __PLAYER__INVENTORY__HH__
#define __PLAYER__INVENTORY__HH__

#include <map>
#include <stdint.h>

namespace beef
{
    struct inventory
    {
        int m_size = 20;
        std::map<uint16_t, uint8_t> m_items;
    };
}

#endif // __PLAYER__INVENTORY__HH__
