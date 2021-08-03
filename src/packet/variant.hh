#ifndef __PACKET__VARIANT__HH__
#define __PACKET__VARIANT__HH__

#include <vector>
#include <string>
#include <iomanip>
#include <variant>

#include "math/vec2.hh"
#include "math/vec3.hh"

#include "packet/gameupdatepacket.hh"

namespace beef
{
    enum
    {
        VARIANT_UNKNOWN,
        VARIANT_FLOAT,
        VARIANT_STR,
        VARIANT_VEC2,
        VARIANT_VEC3,
        VARIANT_UINT,
        VARIANT_INT = 9
    };

    using variant = std::variant<float, std::string, math::vec2, math::vec3, uint32_t, int>;

    class variantlist
    {
    public:
        variantlist(const variant& v0) : m_variants({v0}) {}
        variantlist(const variant& v0, const variant& v1) : m_variants({v0,v1}) {}
        variantlist(const variant& v0, const variant& v1, const variant& v2) : m_variants({v0,v1,v2}) {}
        variantlist(const variant& v0, const variant& v1, const variant& v2, const variant& v3) : m_variants({v0,v1,v2,v3}) {}
        variantlist(const variant& v0, const variant& v1, const variant& v2, const variant& v3, const variant& v4) : m_variants({v0,v1,v2,v3,v4}) {}
        variantlist(const variant& v0, const variant& v1, const variant& v2, const variant& v3, const variant& v4, const variant& v5) : m_variants({v0,v1,v2,v3,v4,v5}) {}
        variantlist(const variant& v0, const variant& v1, const variant& v2, const variant& v3, const variant& v4, const variant& v5, const variant& v6) : m_variants({v0,v1,v2,v3,v4,v5,v6}) {}

        int get_type(const variant& var) const
        {
            switch (var.index())
            {
                case 0:
                    return VARIANT_FLOAT;
                case 1:
                    return VARIANT_STR;
                case 2:
                    return VARIANT_VEC2;
                case 3:
                    return VARIANT_VEC3;
                case 4:
                    return VARIANT_UINT;
                case 5:
                    return VARIANT_INT;
                default:
                    return VARIANT_UNKNOWN;
            }
        }

        // gotta make this cleaner, soon.
        gameupdatepacket* pack() const
        {
            uint32_t size = sizeof(gameupdatepacket) + 1 + (m_variants.size() * 2);

            for (uint8_t i = 0; i < m_variants.size(); ++i)
            {
                const variant& var = m_variants.at(i);
                int type = get_type(var);

                switch (type)
                {
                    case VARIANT_FLOAT:
                    case VARIANT_UINT:
                    case VARIANT_INT:
                        size += 4;
                        break;

                    case VARIANT_STR:
                        size += 4 + std::get<std::string>(var).size();
                        break;

                    case VARIANT_VEC2:
                        size += 8;
                        break;
                        
                    case VARIANT_VEC3:
                        size += 12;
                        break;

                    default:
                        break;
                }
            }

            gameupdatepacket* p = static_cast<gameupdatepacket*>(malloc(size));

            memset(p, 0, size);

            uint8_t* extra = static_cast<uint8_t*>(&p->m_data);
            uint32_t pos = 1;
            uint8_t cnt = 0;

            for (uint8_t i = 0; i < m_variants.size(); ++i)
            {
                const variant& var = m_variants.at(i);
                int type = get_type(var);
                // if (type == variant_type::v_unk) break;

                extra[pos] = static_cast<uint8_t>(i);
                extra[pos + 1] = static_cast<uint8_t>(type);

                switch (type)
                {
                    case VARIANT_FLOAT:
                    {
                        float v_float = std::get<float>(var);
                        memcpy(extra + pos + 2, &v_float, 4);
                        pos += 6;
                        break;
                    }
                    
                    case VARIANT_STR:
                    {
                        std::string v_str = std::get<std::string>(var);
                        uint32_t len = static_cast<uint32_t>(v_str.size());
                        memcpy(extra + pos + 2, &len, 4);
                        memcpy(extra + pos + 6, v_str.data(), len);
                        pos += 6 + len;
                        break;
                    }

                    case VARIANT_VEC2:
                    {
                        math::vec2 v_vec = std::get<math::vec2>(var);
                        memcpy(extra + pos + 2, &v_vec.m_x, 4);
                        memcpy(extra + pos + 6, &v_vec.m_y, 4);
                        pos += 10;
                        break;
                    }

                    case VARIANT_VEC3:
                    {
                        math::vec3 v_vec = std::get<math::vec3>(var);
                        memcpy(extra + pos + 2, &v_vec.m_x, 4);
                        memcpy(extra + pos + 6, &v_vec.m_y, 4);
                        memcpy(extra + pos + 10, &v_vec.m_z, 4);
                        pos += 14;
                        break;
                    }

                    case VARIANT_UINT:
                    {
                        uint32_t v_int = std::get<uint32_t>(var);
                        memcpy(extra + pos + 2, &v_int, 4);
                        pos += 6;
                        break;
                    }

                    case VARIANT_INT:
                    {
                        int v_int = std::get<int>(var);
                        memcpy(extra + pos + 2, &v_int, 4);
                        pos += 6;
                        break;
                    }

                    default:
                        throw std::runtime_error("unknown type of variant");
                        break;
                }

                cnt = i;
            }

            extra[0] = cnt + 1;
            
            p->m_type = 1;
            p->m_net_id = -1;
            p->m_flags |= 0x8;
            p->m_data_size = pos;

            return p;
        }

    private:
        std::vector<variant> m_variants;
    };
}

#endif // __PACKET__VARIANT__HH__
