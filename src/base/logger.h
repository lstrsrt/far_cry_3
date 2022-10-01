#pragma once

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include <Windows.h>

#define log_success(fmt, ...) logger::add<logger::level::success>(fmt, __VA_ARGS__)
#define log_info(fmt, ...) logger::add<logger::level::info>(fmt, __VA_ARGS__)
#define log_error(fmt, ...) logger::add<logger::level::error>(fmt, __VA_ARGS__)
#define log_raw(fmt, ...) logger::add<logger::level::raw>(fmt, __VA_ARGS__)

namespace logger
{
    inline HANDLE m_console{ };
    inline std::string m_log_name{ };

    enum class level
    {
        success,
        info,
        error,
        raw
    };

#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

    template<level lvl, class... va_args>
    void add(const std::string_view fmt, const va_args&... args) noexcept
    {
        const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto msg = std::vformat(fmt, std::make_format_args(args...));

        if constexpr (lvl != level::raw) {
            std::cout << "[ ";
            switch (lvl) {
            case level::success:
                SetConsoleTextAttribute(m_console, FOREGROUND_GREEN);
                std::cout << '+';
                break;
            case level::info:
                SetConsoleTextAttribute(m_console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                std::cout << '*';
                break;
            case level::error:
                SetConsoleTextAttribute(m_console, FOREGROUND_RED);
                std::cout << '!';
                break;
            }

            SetConsoleTextAttribute(m_console, FOREGROUND_WHITE);
            std::tm buf{};
            localtime_s(&buf, &time);
            std::cout << " ] " << std::put_time(&buf, "[%T] ");
        }

        std::cout << msg << '\n';

        std::fstream out{ m_log_name, std::ios::out | std::ios::app };
        if (out.good()) {
            out << msg << '\n';
            out.flush();
        }
    }

#undef FOREGROUND_WHITE

    inline void init(std::string_view console_title, std::string_view log_name) noexcept
    {
        AllocConsole();
        AttachConsole(ATTACH_PARENT_PROCESS);
        SetConsoleTitle(console_title.data());

        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        HANDLE file = CreateFile(m_log_name.c_str(), 0, 0, nullptr, CREATE_NEW, 0, nullptr);

        m_console = GetStdHandle(STD_OUTPUT_HANDLE);
        m_log_name = log_name;

        CloseHandle(file);
    }

    inline void end() noexcept
    {
        fclose(stdout);
        FreeConsole();
    }
}