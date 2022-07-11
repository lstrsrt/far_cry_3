#pragma once

#include <chrono>
#include <thread>

#include "base/memory.h"
#include "base/logger.h"
#include "base/input.h"
#include "hooks.h"
#include "cheat.h"
#include "../ext/MinHook/include/MinHook.h"

namespace cheat
{
    inline module_t m_fc3{ };

    static void detach(HMODULE hmod) noexcept
    {
        hooks::end();
        input::end();
        logger::end();
        FreeLibraryAndExitThread(hmod, EXIT_SUCCESS);
    }

    static DWORD WINAPI init(LPVOID param) noexcept
    {
        logger::init("Far Cry 3", "cheat_log.txt");
        log_success("Logger initialized.");

        input::init();

        m_fc3.m_name = GetModuleHandle("FC3_d3d11.dll") ? "FC3_d3d11.dll" : "FC3.dll";
        m_fc3.init();

        hooks::init();
        log_success("Set up hooks - entering idle loop.");

        log_raw("Infinite ammo: numpad 0");
        log_raw("Infinite money: numpad 1");
        log_raw("Infinite health syringes: numpad 2");
        log_raw("Infinite health: numpad 3");
        log_raw("Fast walk: numpad 4");
        log_raw("Super jump: numpad 5");
        log_raw("No spread + recoil: numpad 6");

        using namespace std::chrono_literals;
        while (!input::is_key_pressed(VK_DELETE))
            std::this_thread::sleep_for(100ms);

        detach(static_cast<HMODULE>(param));

        return 0;
    }
}
