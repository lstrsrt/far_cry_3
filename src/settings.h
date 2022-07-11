#pragma once

#include <array>
#include <mmsystem.h>

struct setting
{
    std::string m_name{ };
    bool m_value{ };
    UINT m_key{ };

    explicit setting(const char* name, UINT key) noexcept
        : m_name(name), m_key(key) { }

    void update() noexcept
    {
        if (input::is_key_pressed(m_key)) {
            m_value = !m_value;
            log_raw("{}: {}", m_name, m_value ? "on" : "off");
            PlaySound("SystemAsterisk", nullptr, SND_ASYNC);
        }
    }

    operator bool() const noexcept { return m_value; }
};

namespace settings
{
    setting m_inf_ammo{ "Infinite ammo", VK_NUMPAD0 };
    setting m_inf_money{ "Infinite money", VK_NUMPAD1 };
    setting m_inf_health_syringes{ "Infinite health syringes", VK_NUMPAD2 };
    setting m_inf_health{ "Infinite health", VK_NUMPAD3 };
    setting m_fast_walk{ "Fast walk", VK_NUMPAD4 };
    setting m_super_jump{ "Super jump", VK_NUMPAD5 };
    setting m_accurate_wpn{ "No spread + recoil", VK_NUMPAD6 };

    void update() noexcept
    {
        m_inf_ammo.update();
        m_inf_money.update();
        m_inf_health_syringes.update();
        m_inf_health.update();
        m_fast_walk.update();
        m_super_jump.update();
        m_accurate_wpn.update();
    }
}
