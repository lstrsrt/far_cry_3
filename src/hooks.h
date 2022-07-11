#pragma once

#include <intrin.h>

#include "sdk.h"
#include "../ext/MinHook/include/MinHook.h"

#define decl_hook(ret, name, base, ...) namespace name { \
    using fn = ret(__thiscall*)(base*, __VA_ARGS__); \
    inline fn original{ }; \
    ret __fastcall hook(base* ecx, int, __VA_ARGS__); \
}

namespace hooks
{
    void init();

    inline void end()
    {
        MH_Uninitialize();
    }

    decl_hook(int, update_current_ammo, sdk::weapon, int)
    decl_hook(void, sub_money, sdk::money, int)
    decl_hook(void, sub_health_syringe, sdk::unknown_struct)
    decl_hook(int, get_life_state, sdk::player)
    decl_hook(void, set_velocity, void, sdk::vec3*)
    decl_hook(float*, update_accuracy, float, uintptr_t)
    decl_hook(double, get_spread, sdk::weapon)
    decl_hook(int, update, sdk::hkp_character_state_jumping, int, __m128*, __m128*)
}

#undef decl_hook
