#ifndef __MATH__VEC2I_HH__
#define __MATH__VEC2I_HH__

namespace beef::math
{
    struct vec2i
    {
        int m_x, m_y;

        vec2i(int x, int y)
            : m_x(x), m_y(y)
        {
        }

        bool operator==(const vec2i& other)
        {
            return m_x == other.m_x && m_y == other.m_y;
        }
    };
}

#endif // __MATH__VEC2I_HH__
