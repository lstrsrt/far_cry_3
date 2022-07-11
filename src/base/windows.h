#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

namespace windows
{
    struct scoped_handle
    {
        HANDLE m_handle{ };

        scoped_handle() noexcept { }
        scoped_handle(HANDLE handle) noexcept
            : m_handle(handle) { }
        ~scoped_handle() noexcept
        {
            if (is_valid())
                CloseHandle(m_handle);
        }

        bool is_valid() const noexcept { return m_handle && m_handle != INVALID_HANDLE_VALUE; }

        operator bool() const noexcept { return is_valid(); }
        operator HANDLE() noexcept { return m_handle; }
    };

    inline bool get_process_id(std::string_view name, DWORD& pid) noexcept
    {
        scoped_handle snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe{ sizeof(PROCESSENTRY32) };
        if (Process32First(snapshot, &pe)) {
            while (Process32Next(snapshot, &pe)) {
                if (!name.compare(pe.szExeFile)) {
                    pid = pe.th32ProcessID;
                    return true;
                }
            }
        }
        return false;
    }
}
