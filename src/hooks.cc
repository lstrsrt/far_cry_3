#include "base/input.h"
#include "base/logger.h"
#include "sdk.h"
#include "hooks.h"
#include "cheat.h"
#include "settings.h"

void hooks::init()
{
    const auto set_hook = [](void* addr, void* detour, void** orig, const char* name) {
        const auto status = MH_CreateHook(addr, detour, orig);
        if (status != MH_OK)
            log_error("Error while hooking function {}: {}", name, MH_StatusToString(status));
    };

    MH_Initialize();

    const auto update_current_ammo = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? EB 06 89 81"), true).cast<void*>();
    log_info("Found UpdateCurrentAmmo -> {}", update_current_ammo);

    const auto sub_reserve_ammo = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? F6 05 ? ? ? ? ? BF ? ? ? ? 75 50"), true).cast<void*>();
    log_info("Found SubReserveAmmo -> {}", sub_reserve_ammo);

    const auto sub_money = cheat::m_fc3.find(PATTERN("E9 ? ? ? ? 33 C0 39 41 64"), true).cast<void*>();
    log_info("Found SubMoney -> {}", sub_money);

    const auto sub_health_syringe = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? DB 45 F8 8B 45 F8 53"), true).cast<void*>();
    log_info("Found SubHealthSyringe -> {}", sub_health_syringe);

    const auto get_life_state = cheat::m_fc3.find(PATTERN("0F 57 C0 33 C0 0F 2F 41 10")).cast<void*>();
    log_info("Found GetLifeState -> {}", get_life_state);

    const auto set_velocity = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 5E 86"), true).cast<void*>();
    log_info("Found SetVelocity -> {}", set_velocity);

    const auto update_accuracy = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? EB 16 FF 76 0C"), true).cast<void*>();
    log_info("Found UpdateAccuracy -> {}", update_accuracy);

    const auto get_spread = cheat::m_fc3.find(PATTERN("E8 ? ? ? ? D9 5D 14 8B 4D 10"), true).cast<void*>();
    log_info("Found GetSpread -> {}", get_spread);

    const auto update = cheat::m_fc3.find(PATTERN("55 8B EC 51 56 8B F1 83 7E 0C 00 57")).cast<void*>();
    log_info("Found hkpCharacterStateJumping::update -> {}", update);

    set_hook(update_current_ammo, update_current_ammo::hook, (void**)&update_current_ammo::original, "UpdateCurrentAmmo");
    set_hook(sub_reserve_ammo, sub_reserve_ammo::hook, (void**)&sub_reserve_ammo::original, "SubReserveAmmo");
    set_hook(sub_money, sub_money::hook, (void**)&sub_money::original, "SubMoney");
    set_hook(sub_health_syringe, sub_health_syringe::hook, (void**)&sub_health_syringe::original, "SubHealthSyringe");
    set_hook(get_life_state, get_life_state::hook, (void**)&get_life_state::original, "GetLifeState");
    set_hook(set_velocity, set_velocity::hook, (void**)&set_velocity::original, "SetVelocity");
    set_hook(update_accuracy, update_accuracy::hook, (void**)&update_accuracy::original, "UpdateAccuracy");
    set_hook(get_spread, get_spread::hook, (void**)&get_spread::original, "GetSpread");
    set_hook(update, update::hook, (void**)&update::original, "hkpCharacterStateJumping::update");

    MH_EnableHook(nullptr);
}

LRESULT CALLBACK input::wnd_proc::hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_KEYDOWN || msg == WM_KEYUP)
        settings::update();

    return CallWindowProc(original, hwnd, msg, wparam, lparam);
}

int __fastcall hooks::update_current_ammo::hook(sdk::weapon* ecx, int, int amount)
{
    return settings::m_inf_ammo ? original(ecx, amount + 1) : original(ecx, amount);
}

void __fastcall hooks::sub_reserve_ammo::hook(sdk::weapon* ecx, int, int amount)
{
    if (settings::m_inf_ammo)
        return;

    return original(ecx, amount);
}

void __fastcall hooks::sub_money::hook(sdk::money* ecx, int, int amount)
{
    if (!settings::m_inf_money)
        return original(ecx, amount);

    ecx->m_value += amount;
}

void __fastcall hooks::sub_health_syringe::hook(sdk::unknown_struct* ecx, int)
{
    if (!settings::m_inf_health_syringes)
        return original(ecx);

    // ecx->m_health_syringes = 
}

int __fastcall hooks::get_life_state::hook(sdk::player* ecx, int)
{
    if (settings::m_inf_health)
        ecx->m_health = 200.f;

    return original(ecx);
}

void __fastcall hooks::set_velocity::hook(void* ecx, int, sdk::vec3* vel)
{
    if (!settings::m_fast_walk)
        return original(ecx, vel);

    auto v2 = (sdk::movement*)((uintptr_t*)ecx)[9];

    if (!v2->m_unknown) {
        v2->m_multiplier = 8.f;

        vel->x *= v2->m_multiplier;
        vel->y *= v2->m_multiplier;
        vel->z *= v2->m_multiplier;

        if (v2->m_velocity.length_sqr() > 0.f) {
            vel->x += v2->m_velocity.x;
            vel->y += v2->m_velocity.y;
            vel->z += v2->m_velocity.z;
        }
    }
}

float* __fastcall hooks::update_accuracy::hook(float* ecx, int, uintptr_t a2)
{
    // taken straight from ida, didn't bother reversing this

    ecx[0] = settings::m_accurate_wpn ? 0.f : *(float*)(a2);
    ecx[1] = settings::m_accurate_wpn ? 0.f : *(float*)(a2 + 4);
    ecx[2] = *(float*)(a2 + 8);
    ecx[3] = *(float*)(a2 + 12);
    ecx[5] = *(float*)(a2 + 20);
    ecx[6] = *(float*)(a2 + 24);
    *((BYTE*)ecx + 16) = *(BYTE*)(a2 + 16);
    ecx[7] = *(float*)(a2 + 28);
    ecx[8] = *(float*)(a2 + 32);

    return ecx;
}

double __fastcall hooks::get_spread::hook(sdk::weapon* ecx, int)
{
    return settings::m_accurate_wpn ? 0. : original(ecx);
}

int __fastcall hooks::update::hook(sdk::hkp_character_state_jumping* ecx, int, int a2, __m128* a3, __m128* a4)
{
    if (settings::m_super_jump)
        ecx->m_jump_height = 50.f;

    return original(ecx, a2, a3, a4);
}
