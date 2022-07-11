#pragma once

#include "base/memory.h"

namespace sdk
{
    struct vec3
    {
        float x;
        float y;
        float z;

        auto dot_product(const vec3& v) const noexcept
        {
            return x * v.x + y * v.y + z * v.z;
        }

        auto length_sqr() const noexcept
        {
            return dot_product(*this);
        }
    };

    struct hkp_character_state_jumping
    {
        PAD(0x8);
        float m_jump_height;
    };

    struct money
    {
        PAD(0x4);
        int m_value;
    };

    struct movement
    {
        PAD(0x734);
        vec3 m_velocity;
        PAD(0x38);
        float m_multiplier;
        PAD(0xd);
        bool m_unknown;
    };

    struct player
    {
        PAD(0x10);
        float m_health;
    };

    struct unknown_struct
    {
        PAD(0x60);
        int m_health_syringes;
    };

    struct weapon
    {
        PAD(0xcc);
        int m_clip;
    };
}
