#ifndef __ITEMDB__ITME_FLAGS__HH__
#define __ITEMDB__ITME_FLAGS__HH__

namespace beef
{
    enum
    {
        ITEMFLAG1_FLIPPED = 1 << 0,
        ITEMFLAG1_EDITABLE = 1 << 1,
        ITEMFLAG1_SEEDLESS = 1 << 2,
        ITEMFLAG1_PERMANENT = 1 << 3,
        ITEMFLAG1_DROPLESS = 1 << 4,
        ITEMFLAG1_NOSELF = 1 << 5,
        ITEMFLAG1_NOSHADOW = 1 << 6,
        ITEMFLAG1_WORLD_LOCK = 1 << 7
    };

    enum
    {
        ITEMFLAG2_BETA = 1 << 0,
        ITEMFLAG2_AUTOPICKUP = 1 << 1,
        ITEMFLAG2_MOD = 1 << 2,
        ITEMFLAG2_RANDGROW = 1 << 3,
        ITEMFLAG2_PUBLIC = 1 << 4,
        ITEMFLAG2_FOREGROUND = 1 << 5,
        ITEMFLAG2_HOLIDAY = 1 << 6,
        ITEMFLAG2_UNTRADABLE = 1 << 7,
    };
}

#endif // __ITEMDB__ITME_FLAGS__HH__
