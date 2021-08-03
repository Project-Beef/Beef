#ifndef __MATH__VEC3__HH__
#define __MATH__VEC3__HH__

namespace beef::math
{
    struct vec3
    {
        float m_x, m_y, m_z;

        vec3(float x, float y, float z)
            : m_x(x), m_y(y), m_z(z)
        {
        }

        vec3(int x, int y, int z)
        {
            m_x = static_cast<float>(x);
            m_y = static_cast<float>(y);
            m_z = static_cast<float>(z);
        }
    };
}

#endif // __MATH__VEC2__HH__
