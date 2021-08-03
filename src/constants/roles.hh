#ifndef __CONSTANTS__ADMIN_LVL__HH__
#define __CONSTANTS__ADMIN_LVL__HH__

#include <unordered_map>

namespace beef
{
    enum
    {
        LVL_NORMAL = 0,
        LVL_BETA_TESTER = 1,
        LVL_WORLD_ADMIN = 25,
        LVL_WORLD_OWNER = 50,
        LVL_VIP = 200,
        LVL_VIP_PLUS = 250,
        LVL_ROYAL = 300,
        LVL_PERSONAL = 400,
        LVL_MODERATOR = 500,
        LVL_MANAGER = 1000,
        LVL_DEVELOPER = 5000,
        LVL_OWNER = 10000,
    };

    static std::unordered_map<int, char> name_color = {
        { LVL_BETA_TESTER, 'y' },
        { LVL_WORLD_ADMIN, '^' },
        { LVL_WORLD_OWNER, '2' },
        { LVL_VIP, '1' },
        { LVL_VIP, '3' },
        { LVL_ROYAL, '4' },
        { LVL_PERSONAL, '5' },
        { LVL_MODERATOR, '#' },
        { LVL_MANAGER, 'r' },
        { LVL_DEVELOPER, '6' },
        { LVL_OWNER, '6' }
    };
}

#endif // __CONSTANTS__ADMIN_LVL__HH__
