#pragma once

#include <array>
#include <Windows.h>

#include "logger.h"

namespace input
{
    inline HWND m_hwnd{ };
    inline std::array<bool, 255> m_keys{ };

    static __declspec(noinline) bool is_key_pressed(uint32_t code) noexcept
    {
        return m_keys[code];
    }

    namespace wnd_proc
    {
        inline LRESULT(CALLBACK* original)(HWND, UINT, WPARAM, LPARAM);
        extern LRESULT CALLBACK hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    }

    static void init() noexcept
    {
        m_hwnd = FindWindow("Nomad", nullptr);
        if (!m_hwnd) {
            log_error("Could not find target window.");
            return;
        }

        wnd_proc::original = reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc::hook)));
        if (!wnd_proc::original)
            log_error("Could not redirect window message handler.");

        log_success("Redirected window message handler.");
    }

    inline void end() noexcept
    {
        SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc::original));
    }
};
