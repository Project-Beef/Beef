#ifndef __MATH__VEC2__HH__
#define __MATH__VEC2__HH__

namespace beef::math
{
    struct vec2
    {
        float m_x, m_y;

        vec2(float x, float y)
            : m_x(x), m_y(y)
        {
        }
    };
}

#endif // __MATH__VEC2__HH__
