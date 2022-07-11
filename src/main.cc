#include <iostream>
#include <thread>

#include "cheat.h"

BOOL APIENTRY DllMain(HMODULE instance, DWORD call_reason, LPVOID)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        if (const auto thread = CreateThread(nullptr, 0, cheat::init, instance, 0, nullptr))
            CloseHandle(thread);
    }

    return TRUE;
}
