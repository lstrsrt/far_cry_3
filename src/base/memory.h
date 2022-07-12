#pragma once

#include <string_view>

#include "windows.h"
#include "pattern.h"
#include "logger.h"

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define PAD(size) std::byte CONCAT(pad, __COUNTER__)[size]{ };

struct address
{
    uintptr_t m_value{ };

    constexpr explicit address() noexcept = default;
    address(uintptr_t value) noexcept
        : m_value(value) { }
    address(uint8_t* value) noexcept
        : m_value(reinterpret_cast<uintptr_t>(value)) { }

    template<class ty>
    ty absolute(ptrdiff_t offset = 0) const noexcept
    {
        const auto disp = deref<int32_t>();
        if (disp)
            return reinterpret_cast<ty>(m_value + offset + 4 + disp);

        return ty();
    }

    address offset(ptrdiff_t offset) const noexcept
    {
        return address(m_value + offset);
    }

    template<class ty>
    ty cast() const noexcept
    {
        return reinterpret_cast<ty>(m_value);
    }

    template<class ty>
    ty deref() const noexcept
    {
        return *cast<ty*>();
    }

    template<class ty>
    ty deref(size_t count) const noexcept
    {
        auto ret = *cast<ty*>();
        while (--count)
            ret = *reinterpret_cast<ty*>(ret);
        return ret;
    }

    operator address() noexcept { return m_value; }
    bool operator ==(void* rhs) const noexcept { return cast<void*>() == rhs; }
};

struct module_t
{
    std::string m_name{ };
    HMODULE m_handle{ };
    MODULEINFO m_info{ };

    constexpr explicit module_t() noexcept = default;
    explicit module_t(std::string_view name) noexcept
        : m_name(name)
    {
        init();
    }

    void init() noexcept
    {
        m_handle = GetModuleHandle(m_name.data());
        if (m_handle) {
            K32GetModuleInformation(GetCurrentProcess(), m_handle, &m_info, sizeof(m_info));
            log_info("{} -> {} (size: 0x{:x})", m_name, m_info.lpBaseOfDll, m_info.SizeOfImage);
        }
        else
            log_error("Invalid module handle for {}!", m_name);
    }

    template<size_t len>
    address find(std::array<int, len>&& pattern, bool relative = false, ptrdiff_t rel_offset = 0x1) noexcept
    {
        static_assert(len > 0);

        static int i{ };
        i++;

        const auto bytes = reinterpret_cast<uint8_t*>(m_handle);
        for (size_t i{ }; i < m_info.SizeOfImage - len; i++) {
            for (size_t j{ }; j < len; j++) {
                if (bytes[i + j] != pattern[j] && pattern[j] != -1)
                    break;
                if (j + 1 == len)
                    return relative ? address(*reinterpret_cast<int32_t*>(&bytes[i] + rel_offset) + &bytes[i] + rel_offset + 4)
                        : address(&bytes[i]);
            }
        }

        log_error("Did not find pattern {} in module {}!", i, m_name);
        return address();
    }
};
